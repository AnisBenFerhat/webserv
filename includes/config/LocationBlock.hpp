/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 15:03:06 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/03 10:28:23 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>

/**
 * @brief Stores configuration rules for a specific URL prefix (route).
 *
 * Defines how the server should handle requests to a specific location,
 * including which HTTP methods are allowed, where the files are located on
 * disk, and whether directory listing is enabled.
 */
class LocationBlock {
public:
	LocationBlock();
	LocationBlock(const LocationBlock& other);
	LocationBlock& operator=(const LocationBlock& other);
	~LocationBlock();

	// --- Getters ---

	/** @brief Returns the URI prefix defined for this location block. */
	const std::string& getPath() const {
		return _path;
	}

	/** @brief Returns the physical directory path mapped to this location.
	 */
	const std::string& getRoot() const {
		return _root;
	}

	/** @brief Returns the default file name to serve if the URI is a
	 * directory. */
	const std::string& getIndex() const {
		return _index;
	}

	/** @brief Returns a list of allowed HTTP methods (e.g., GET, POST). */
	const std::vector<std::string>& getMethods() const {
		return _methods;
	}

	/** @brief Checks if directory listing (autoindex) is enabled. */
	bool getAutoindex() const {
		return _autoindex;
	}

	/**
		 * @brief Returns the file extension that triggers CGI execution.
		 * @return A constant reference to the CGI extension string (e.g., ".py").
	*/
	const std::string& getCgiExtension() const {
		return _cgiExtension;
	}

	/**
		 * @brief Returns the path to the CGI interpreter binary.
		 * @return A constant reference to the CGI interpreter path string (e.g, "/usr/bin/python3").
	*/
	const std::string& getCgiInterpreter() const {
		return _cgiInterpreter;
	}

	// --- Setters ---

	/** @brief Sets the URI prefix (e.g., "/api" or "/images"). */
	void setPath(const std::string& path) {
		_path = path;
	}

	/** @brief Sets the base directory on the file system for this location.
	 */
	void setRoot(const std::string& root) {
		_root = root;
	}

	/** @brief Sets the default file (e.g., "index.html"). */
	void setIndex(const std::string& index) {
		_index = index;
	}

	/** @brief Adds an allowed HTTP method to the whitelist. */
	void addMethod(const std::string& method) {
		_methods.push_back(method);
	}

	/**
	 * @brief Enables or disables directory listing.
	 * @param state Set to true to enable autoindex.
	 */
	void setAutoindex(bool state) {
		_autoindex = state;
	}

	/**
		 * @brief Sets the file extension that triggers CGI execution.
		 * @param ext The file extension (e.g., ".py").
	*/
	void setCgiExtension(const std::string& extension) {
		_cgiExtension = extension;
	}

	/**
		 * @brief Sets the path to the CGI interpreter binary.
		 * @param interpreter The path to the executable (e.g., "/usr/bin/python3").
	*/
	void setCgiInterpreter(const std::string& interpreter) {
		_cgiInterpreter = interpreter;
	}

private:
	std::string _path;	 ///< URI prefix matching this block.
	std::string _root;	 ///< Root directory for file lookups.
	std::string _index;	 ///< Default file name for directory requests.

	/// Whitelist of supported HTTP methods.
	std::vector<std::string> _methods;

	bool _autoindex;  ///< Toggle for directory listing.

	std::string _cgiExtension;    ///< Extension triggering CGI (--> ".py")
	std::string _cgiInterpreter;  ///< Interpreter binary path (--> "/usr/bin/python3").
};
#endif
