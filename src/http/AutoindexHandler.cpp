/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoindexHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 18:40:07 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/15 11:35:06 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/AutoindexHandler.hpp"
#include "http/MimeTypes.hpp"
#include "http/HttpStatus.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <sstream>
#include <vector>

static const std::string AUTOINDEX_STYLE =
	"        body { font-family: sans-serif; background: #f5f5f5; "
	"color: #333; padding: 40px; }\n"
	"        h1 { font-size: 22px; font-weight: 500; "
	"margin-bottom: 24px; }\n"
	"        table { width: 100%; border-collapse: collapse; "
	"background: #fff; border-radius: 8px; overflow: hidden; "
	"box-shadow: 0 1px 4px rgba(0,0,0,0.08); }\n"
	"        th { text-align: left; padding: 12px 16px; "
	"font-size: 12px; text-transform: uppercase; "
	"letter-spacing: 0.06em; color: #888; "
	"border-bottom: 1px solid #eee; }\n"
	"        td { padding: 12px 16px; font-size: 14px; "
	"border-bottom: 1px solid #f0f0f0; }\n"
	"        tr:last-child td { border-bottom: none; }\n"
	"        tr:hover td { background: #fafafa; }\n"
	"        a { color: #1d4ed8; text-decoration: none; }\n"
	"        a:hover { text-decoration: underline; }\n"
	"        .icon { margin-right: 8px; }\n"
	"        p { font-size: 13px; color: #888; margin-top: 20px; }\n";

HttpResponse AutoindexHandler::createResponse(const std::string& rootPath,
											  const std::string& uri) {
	DIR* dir = opendir(rootPath.c_str());

	if (!dir) {
		HttpResponse res;
		res.setStatus(HTTP_403_FORBIDDEN);
		res.setHeader("Content-Type", "text/html");
		res.setBody("<html><body><h1>403 Forbidden</h1></body></html>");

		return res;
	}

	std::vector<std::string> dirs;
	std::vector<std::string> files;

	struct dirent* entry;

	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;

		if (name == "." || name == "..")
			continue;

		struct stat status;
		std::string fullPath = rootPath + "/" + name;

		if (stat(fullPath.c_str(), &status) != 0)
			continue;

		if (S_ISDIR(status.st_mode))
			dirs.push_back(name);
		else
			files.push_back(name);
	}
	closedir(dir);

	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());

	std::string body = _buildHtml(uri, dirs, files);

	std::ostringstream contentLength;
	contentLength << body.size();

	HttpResponse res;
	res.setStatus(HTTP_200_OK);
	res.setHeader("Content-Type", "text/html");
	res.setHeader("Content-Length", contentLength.str());
	res.setBody(body);

	return res;
}

std::string AutoindexHandler::_buildHtml(
	const std::string& uri, const std::vector<std::string>& dirs,
	const std::vector<std::string>& files) {
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
		 << "<html lang=\"en\">\n"
		 << "<head>\n"
		 << "    <meta charset=\"UTF-8\">\n"
		 << "    <title>Index of " << uri << "</title>\n"
		 << "    <style>\n"
		 << AUTOINDEX_STYLE << "    </style>\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "    <h1>Index of " << uri << "</h1>\n"
		 << "    <table>\n"
		 << "        <thead>\n"
		 << "            <tr><th>Name</th></tr>\n"
		 << "        </thead>\n"
		 << "        <tbody>\n";

	if (uri != "/") {
		html << "            <tr><td>"
			 << "<span class=\"icon\">📁</span>"
			 << "<a href=\"" << uri << "../\">../</a>"
			 << "</td></tr>\n";
	}

	for (size_t i = 0; i < dirs.size(); ++i) {
		std::string name = dirs[i];
		std::string href = uri + name + "/";
		html << "            <tr><td>"
			 << "<span class=\"icon\">📁</span>"
			 << "<a href=\"" << href << "\">" << name << "/</a>"
			 << "</td></tr>\n";
	}

	for (size_t i = 0; i < files.size(); ++i) {
		std::string name = files[i];
		std::string href = uri + name;
		html << "            <tr><td>"
			 << "<span class=\"icon\">" << _getIcon(name) << "</span>"
			 << "<a href=\"" << href << "\">" << name << "</a>"
			 << "</td></tr>\n";
	}

	html << "        </tbody>\n"
		 << "    </table>\n"
		 << "    <p>Webserv</p>\n"
		 << "</body>\n"
		 << "</html>\n";

	return html.str();
}

std::string AutoindexHandler::_getIcon(const std::string& filename) {
	std::string mime = MimeTypes::getType(filename);

	if (mime.find("text/") == 0)
		return "📄";
	if (mime.find("image/") == 0)
		return "🖼️";
	if (mime.find("audio/") == 0)
		return "🎵";
	if (mime.find("video/") == 0)
		return "🎬";
	if (mime.find("application/zip") != std::string::npos ||
		mime.find("application/gzip") != std::string::npos ||
		mime.find("application/x-tar") != std::string::npos)
		return "📦";
	if (mime.find("application/pdf") != std::string::npos)
		return "📋";

	return "❓";
}
