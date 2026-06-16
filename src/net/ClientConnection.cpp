/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:15:27 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/15 14:54:49 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/ClientConnection.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <sstream>

#include "cgi/CgiResponseParser.hpp"
#include "config/Config.hpp"
#include "errors/ErrorPageGenerator.hpp"
#include "http/Dispatcher.hpp"
#include "http/HttpRequestParser.hpp"
#include "http/RequestRouter.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

static HttpResponse generateErrorResponse(HttpStatus status,
                                          const Config &config) {
        ErrorPageGenerator generator(config);
        return generator.createResponse(status);
}

void ClientConnection::handleRead() {
        if (_status == Closing) return;

        if (!_receiveToBuffer()) return;

        updateTimestamp();
        _processHttpRequest();
}

bool ClientConnection::_receiveToBuffer() {
        char tmpBuffer[1024];

        while (true) {
                ssize_t bytesRead =
                    recv(_fd->getRawFd(), tmpBuffer, sizeof(tmpBuffer), 0);

                if (bytesRead < 0) {
                        int logErrno = errno;
                        if (logErrno == EAGAIN || logErrno == EWOULDBLOCK)
                                break;
                        Logger::logWarning(
                            "Read error on FD: " +
                            Convertor::intToStr(_fd->getRawFd()) + " - " +
                            std::string(strerror(logErrno)));
                        _status = Closing;
                        _poller->removeClient(_fd->getRawFd());
                        return false;
                }
                if (bytesRead == 0) {
                        Logger::logInfo("Client closed connection on FD: " +
                                        Convertor::intToStr(_fd->getRawFd()));
                        _status = Closing;
                        _poller->removeClient(_fd->getRawFd());
                        return false;
                }
                _readBuffer.insert(_readBuffer.end(), tmpBuffer,
                                   tmpBuffer + bytesRead);
                _status = ReadingRequest;
        }
        return true;
}

void ClientConnection::_processHttpRequest() {
        if (_cgiProcessing) {
                return;
        }
        while (!_readBuffer.empty()) {
                HttpRequest request;
                std::size_t bytesParsed = 0;
                HttpRequestParser::ParseResult result =
                    HttpRequestParser::parse(_readBuffer, request, bytesParsed);

                if (result == HttpRequestParser::INCOMPLETE) {
                        Logger::logInfo("Partial request on FD: " +
                                        Convertor::intToStr(_fd->getRawFd()) +
                                        " — waiting for more data");
                        break;
                }

                if (result == HttpRequestParser::ERROR) {
                        Logger::logWarning(
                            "Malformed HTTP request on FD: " +
                            Convertor::intToStr(_fd->getRawFd()));
                        _sendBadRequest();
                        _readBuffer.clear();
                        return;
                }

                if (!_serverBlk) {
                        Logger::logError("No ServerBlock attached to FD: " +
                                         Convertor::intToStr(_fd->getRawFd()));
                        _status = Closing;
                        _poller->removeClient(_fd->getRawFd());
                        return;
                }

                const std::string &hostHeader = request.getHeader("Host");
                const Config *config = _serverBlk->getConfigForHost(hostHeader);

                if (!config) {
                        Logger::logWarning("No config found for host: [" +
                                           hostHeader + "]");
                        _status = Closing;
                        _poller->removeClient(_fd->getRawFd());
                        return;
                }

                const LocationBlock *location =
                    RequestRouter::matchLocation(*config, request);

                HttpResponse response;
                _isKeepAlive = (request.getHeader("Connection") != "close");
                if (!location) {
                        Logger::logWarning("No location matched URI: [" +
                                           request.getPath() + "]");
                        response =
                            generateErrorResponse(HTTP_404_NOT_FOUND, *config);
                } else {
                        std::string fullPath =
                            Dispatcher::_resolvePath(request, *location);
                        // DYNAMIC CGI REQUEST
                        if (Dispatcher::_isCgiRequest(fullPath, *location)) {
                                const std::string &interpreter =
                                    location->getCgiInterpreter();
                                if (interpreter.empty()) {
                                        response = generateErrorResponse(
                                            HTTP_500_INTERNAL_SERVER_ERROR,
                                            *config);
                                } else {
                                        CgiHandler *handler = new CgiHandler();
                                        int outPipe = handler->launchCgiProcess(
                                            request, fullPath, interpreter);

                                        if (outPipe < 0) {
                                                response =
                                                    generateErrorResponse(
                                                        HTTP_502_BAD_GATEWAY,
                                                        *config);
                                                delete handler;
                                        } else {
                                                _cgiStart = time(NULL);
                                                _config = config;
                                                this->setCgiFields(
                                                    handler->getStdin(),
                                                    handler->getStdout(),
                                                    handler->getPid(),
                                                    request.getBody());
                                                _activeCgi = handler;
                                                _poller->addFd(
                                                    _cgiOut->getRawFd(),
                                                    POLLIN);
                                                _poller->getLookupTable()
                                                    .insertCgiPipe(
                                                        _cgiOut->getRawFd(),
                                                        this);

                                                if (!_cgiBuffer.empty()) {
                                                        _poller->addFd(
                                                            _cgiIn->getRawFd(),
                                                            POLLOUT);
                                                        _poller
                                                            ->getLookupTable()
                                                            .insertCgiPipe(
                                                                _cgiIn
                                                                    ->getRawFd(),
                                                                this);
                                                } else {
                                                        if (_cgiIn) {
                                                                delete _cgiIn;
                                                                _cgiIn = NULL;
                                                        }
                                                }
                                                _readBuffer.erase(
                                                    _readBuffer.begin(),
                                                    _readBuffer.begin() +
                                                        bytesParsed);
                                                _cgiProcessing = true;
                                                return;
                                        }
                                }
                        }
                        // STATIC FILE REQUEST
                        else {
                                response = Dispatcher::dispatch(
                                    request, *location, *config);
                                std::string serialized = response.serialize();
                                _writeBuffer.insert(_writeBuffer.end(),
                                                    serialized.begin(),
                                                    serialized.end());

                                _readBuffer.erase(
                                    _readBuffer.begin(),
                                    _readBuffer.begin() + bytesParsed);
                                if (request.getHeader("Connection") ==
                                        "close" ||
                                    response.getHeader("Connection") ==
                                        "close") {
                                        _status = Closing;
                                } else {
                                        _status = KeepAliveWait;
                                }
                        }
                }
        }
        if (!_writeBuffer.empty()) {
                if (_status != Closing) {
                        _status = WritingResponse;
                }
                _poller->setEvents(_fd->getRawFd(), POLLOUT);
        }
}

void ClientConnection::_sendBadRequest() {
        std::string badRequest =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 50\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>400 Bad Request</h1></body></html>";
        _writeBuffer.insert(_writeBuffer.end(), badRequest.begin(),
                            badRequest.end());
        _status = WritingResponse;
        _poller->setEvents(_fd->getRawFd(), POLLOUT);
}

void ClientConnection::handleWrite() {
        if (_writeBuffer.empty()) {
                _poller->setEvents(_fd->getRawFd(), POLLIN);
                return;
        }

        bool yieldToPoll = false;

        while (_writeOffset < _writeBuffer.size()) {
                std::size_t remaining = _writeBuffer.size() - _writeOffset;
                std::size_t toSend = remaining < 65536 ? remaining : 65536;

                int bytesSent = send(_fd->getRawFd(),
                                     &_writeBuffer[_writeOffset], toSend, 0);
                if (bytesSent < 0) {
                        int logErrno = errno;
                        if (logErrno == EAGAIN || logErrno == EWOULDBLOCK) {
                                yieldToPoll = true;
                                break;
                        }
                        Logger::logWarning(
                            "Failed while writing into socket FD: " +
                            Convertor::intToStr(_fd->getRawFd()) + " - " +
                            std::string(strerror(logErrno)));
                        _status = Closing;
                        _poller->removeClient(_fd->getRawFd());
                        return;
                }
                if (bytesSent == 0) {
                        Logger::logWarning(
                            "No bytes sent on FD: " +
                            Convertor::intToStr(_fd->getRawFd()));
                        _status = Closing;
                        _poller->removeClient(_fd->getRawFd());
                        return;
                }
                _writeOffset += static_cast<std::size_t>(bytesSent);
                updateTimestamp();
                if (static_cast<std::size_t>(bytesSent) < toSend) {
                        yieldToPoll = true;
                        break;
                }
                Logger::logInfo("Successfully sent packet into socket FD: " +
                                Convertor::intToStr(_fd->getRawFd()));
        }
        if (yieldToPoll) {
                _poller->setEvents(_fd->getRawFd(), POLLOUT);
        } else if (_writeOffset >= _writeBuffer.size()) {
                _writeBuffer.clear();
                _writeOffset = 0;
                if ((_status == KeepAliveWait || _status == WritingResponse) &&
                    _isKeepAlive) {
                        _isKeepAlive = true;
                        _status = InitialState;
                        _poller->setEvents(_fd->getRawFd(), POLLIN);
                        Logger::logInfo("Response fully sent on FD: " +
                                        Convertor::intToStr(_fd->getRawFd()));
                        if (!_readBuffer.empty()) {
                                Logger::logInfo(
                                    "Pipelined request detected in FD: " +
                                    Convertor::intToStr(_fd->getRawFd()) +
                                    ". Processing now.");
                                _processHttpRequest();
                        }
                } else {
                        Logger::logInfo("Response fully sent on FD: " +
                                        Convertor::intToStr(_fd->getRawFd()) +
                                        " - Closing connection.");
                        _poller->removeClient(_fd->getRawFd());
                }
        }
}

void ClientConnection::setCgiFields(Fd *cgiIn, Fd *cgiOut, pid_t cgiPid,
                                    const std::string &body) {
        _cgiIn = cgiIn;
        _cgiOut = cgiOut;
        _cgiPid = cgiPid;
        _cgiBuffer = body;
}

void ClientConnection::_cleanupCgi() {
        if (_cgiIn) {
                _poller->removeFd(_cgiIn->getRawFd());
                _poller->getLookupTable().removeFd(_cgiIn->getRawFd());
                delete _cgiIn;
                _cgiIn = NULL;
        }
        if (_cgiOut) {
                _poller->removeFd(_cgiOut->getRawFd());
                _poller->getLookupTable().removeFd(_cgiOut->getRawFd());
                delete _cgiOut;
                _cgiOut = NULL;
        }
        _cgiPid = -1;
        _cgiBuffer.clear();
}

void ClientConnection::cgiTimeout(time_t current, int timeout) {
        if (_cgiPid < 0) {
                return;
        }
        if (current - _cgiStart < timeout) {
                return;
        }
        Logger::logWarning("CGI timeout: " +
                           Convertor::intToStr(_fd->getRawFd()));
        kill(_cgiPid, SIGKILL);
        int status = 0;
        waitpid(_cgiPid, &status, 0);
        _cgiResponse.clear();
        HttpResponse response =
            generateErrorResponse(HTTP_504_GATEWAY_TIMEOUT, *_config);
        std::string serialized = response.serialize();
        _writeBuffer.insert(_writeBuffer.end(), serialized.begin(),
                            serialized.end());
        _cgiProcessing = false;
        _cleanupCgi();
        if (_activeCgi) {
                delete _activeCgi;
                _activeCgi = NULL;
        }
        _status = WritingResponse;
        _poller->setEvents(_fd->getRawFd(), POLLOUT);
}

void ClientConnection::cgiRead(int pipeFd) {
        if (!_cgiProcessing) {
                return;
        }
        Logger::logInfo("cgiRead() triggered on Pipe FD: " +
                        Convertor::intToStr(pipeFd));
        char buffer[4096];
        if (!_cgiOut) {
                return;
        }
        ssize_t bytesRead = read(_cgiOut->getRawFd(), buffer, sizeof(buffer));
        if (bytesRead < 0) {
                int logErrno = errno;
                if (logErrno == EAGAIN || logErrno == EWOULDBLOCK) {
                        return;
                }
                Logger::logWarning("CGI read error.");
                _status = Closing;
                _cgiProcessing = false;
                _cleanupCgi();
                _poller->removeClient(_fd->getRawFd());
                return;
        }
        if (bytesRead == 0) {
                _poller->removeFd(_cgiOut->getRawFd());
                _poller->getLookupTable().removeFd(_cgiOut->getRawFd());
                delete _cgiOut;
                _cgiOut = NULL;

                int child = 0;
                waitpid(_cgiPid, &child, WNOHANG);
                HttpResponse response =
                    CgiResponseParser::createResponse(_cgiResponse);
                _cgiResponse.clear();
                std::string serialized = response.serialize();
                _writeBuffer.insert(_writeBuffer.end(), serialized.begin(),
                                    serialized.end());
                _cgiProcessing = false;
                _cleanupCgi();
                if (_activeCgi) {
                        delete _activeCgi;
                        _activeCgi = NULL;
                }
                _status = WritingResponse;
                _poller->setEvents(_fd->getRawFd(), POLLOUT);
                Logger::logInfo(
                    "CGI script execution completed for client FD: " +
                    Convertor::intToStr(_fd->getRawFd()));
                return;
        }
        if (bytesRead > 0) {
                _cgiResponse.append(buffer,
                                    static_cast<std::size_t>(bytesRead));
                updateTimestamp();
                return;
        }
}

void ClientConnection::cgiWrite(int pipeFd) {
        if (!_cgiProcessing) {
                return;
        }
        if (_cgiBuffer.empty()) {
                _poller->removeFd(pipeFd);
                _poller->getLookupTable().removeFd(pipeFd);
                if (_cgiIn) {
                        delete _cgiIn;
                        _cgiIn = NULL;
                }
                return;
        }
        ssize_t bytesWritten =
            write(pipeFd, _cgiBuffer.data(), _cgiBuffer.size());
        if (bytesWritten < 0) {
                int logErrno = errno;
                if (logErrno == EAGAIN || logErrno == EWOULDBLOCK) {
                        return;
                }
                Logger::logWarning("CGI stdin write error on FD: " +
                                   Convertor::intToStr(pipeFd));
                _status = Closing;
                _cgiProcessing = false;
                _cleanupCgi();
                _poller->removeClient(_fd->getRawFd());
                return;
        }
        if (bytesWritten > 0) {
                _cgiBuffer.erase(0, static_cast<std::size_t>(bytesWritten));
                updateTimestamp();
        }
        if (_cgiBuffer.empty()) {
                _poller->removeFd(pipeFd);
                _poller->getLookupTable().removeFd(pipeFd);
                if (_cgiIn) {
                        delete _cgiIn;
                        _cgiIn = NULL;
                }
        }
}

ClientConnection::ClientConnection()
    : _fd(NULL),
      _cgiIn(NULL),
      _cgiOut(NULL),
      _cgiPid(-1),
      _serverBlk(NULL),
      _poller(NULL),
      _status(InitialState),
      _isKeepAlive(false),
      _writeOffset(0),
      _activeCgi(NULL),
      _cgiProcessing(false),
      _cgiStart(0),
      _config(NULL),
      _timeActive(time(NULL)) {}

ClientConnection::ClientConnection(Fd *fd, const ServerBlock *serverBlk,
                                   Poller *poller)
    : _fd(fd),
      _cgiIn(NULL),
      _cgiOut(NULL),
      _cgiPid(-1),
      _serverBlk(serverBlk),
      _poller(poller),
      _status(InitialState),
      _isKeepAlive(false),
      _writeOffset(0),
      _activeCgi(NULL),
      _cgiProcessing(false),
      _cgiStart(0),
      _config(NULL),
      _timeActive(time(NULL)) {
        if (!_fd) Logger::logError("ClientConnection: null Fd pointer");
        if (!_serverBlk)
                Logger::logError("ClientConnection: null ServerBlock pointer");
        if (!_poller) Logger::logError("ClientConnection: null Poller pointer");
}

ClientConnection::~ClientConnection() {
        delete _fd;
        _fd = NULL;
        if (_cgiIn) {
                delete _cgiIn;
                _cgiIn = NULL;
        }
        if (_cgiOut) {
                delete _cgiOut;
                _cgiOut = NULL;
        }
        if (_activeCgi) {
                delete _activeCgi;
                _activeCgi = NULL;
        }
        if (_cgiPid > 0) {
                int status = 0;
                if (waitpid(_cgiPid, &status, WNOHANG) == 0) {
                        kill(_cgiPid, SIGKILL);
                        waitpid(_cgiPid, &status, 0);
                }
        }
}
