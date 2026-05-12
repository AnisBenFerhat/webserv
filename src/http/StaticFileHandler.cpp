/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 20:27:53 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/11 13:41:40 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/StaticFileHandler.hpp"
#include "http/MimeTypes.hpp"
#include <fstream>
#include <sstream>

StaticFileHandler::StaticFileHandler() {}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& other) {
	(void)other;
}

StaticFileHandler& StaticFileHandler::operator=(
	const StaticFileHandler& other) {
	(void)other;
	return *this;
}

StaticFileHandler::~StaticFileHandler() {}

HttpResponse StaticFileHandler::createResponse(const std::string& path) const {
	std::string content = _readFile(path);

	std::ostringstream contentLength;
	contentLength << content.size();

	HttpResponse response;
	response.setStatus(HTTP_200_OK);
	response.setHeader("Content-Type", MimeTypes::getType(path));
	response.setHeader("Content-Length", contentLength.str());
	response.setBody(content);

	return response;
}

std::string StaticFileHandler::_readFile(const std::string& path) const {
	std::ifstream file(path.c_str(), std::ios::binary);

	if (!file.is_open())
		return "";

	std::ostringstream oss;
	oss << file.rdbuf();
	return oss.str();
}
