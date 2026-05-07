/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 08:28:44 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/07 14:39:50 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/MimeTypes.hpp"
#include <cctype>

struct MimeEntry {
		const char* extension;
		const char* mimeType;
};

static const MimeEntry MIME_TABLE[] = {
	// Text and Web
	{"html", "text/html"},
	{"htm", "text/html"},
	{"css", "text/css"},
	{"js", "application/javascript"},
	{"json", "application/json"},
	{"xml", "application/xml"},
	{"csv", "text/csv"},
	{"txt", "text/plain"},

	// Images
	{"png", "image/png"},
	{"jpg", "image/jpeg"},
	{"jpeg", "image/jpeg"},
	{"gif", "image/gif"},
	{"svg", "image/svg+xml"},
	{"ico", "image/x-icon"},
	{"webp", "image/webp"},
	{"bmp", "image/bmp"},
	{"tiff", "image/tiff"},

	// Fonts
	{"woff", "font/woff"},
	{"woff2", "font/woff2"},
	{"ttf", "font/ttf"},
	{"otf", "font/otf"},
	{"eot", "application/vnd.ms-fontobject"},

	// Audio / Video
	{"mp3", "audio/mpeg"},
	{"mp4", "video/mp4"},
	{"mpeg", "video/mpeg"},
	{"ogg", "audio/ogg"},
	{"wav", "audio/wav"},
	{"webm", "video/webm"},
	{"avi", "video/x-msvideo"},

	// Documents
	{"pdf", "application/pdf"},

	// Archives
	{"zip", "application/zip"},
	{"gz", "application/gzip"},
	{"tar", "application/x-tar"},

	// Binaries
	{"bin", "application/octet-stream"},
	{"exe", "application/octet-stream"},

	{NULL, NULL}};

static std::string extractExtension(const std::string& path) {
	size_t dotPosition	 = path.rfind('.');
	size_t slashPosition = path.rfind('/');

	if (dotPosition == std::string::npos)
		return "";

	if (slashPosition != std::string::npos && slashPosition > dotPosition)
		return "";

	std::string extract = path.substr(dotPosition + 1);

	for (size_t i = 0; i < extract.size(); ++i)
		extract[i] = static_cast<char>(
			std::tolower(static_cast<unsigned char>(extract[i])));

	return extract;
}

std::string MimeTypes::getType(const std::string& path) {
	std::string extract = extractExtension(path);

	for (int i = 0; MIME_TABLE[i].extension != NULL; ++i) {
		if (extract == MIME_TABLE[i].extension)
			return MIME_TABLE[i].mimeType;
	}
	return "application/octet-stream";
}
