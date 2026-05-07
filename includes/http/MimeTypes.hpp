/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 08:28:32 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/07 14:39:44 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include <string>

class MimeTypes {
	public:
		/**
		 * @brief Resolves the MIME type for a file path or a filename.
		 **/
		static std::string getType(const std::string& path);

	private:
		MimeTypes();
		MimeTypes(const MimeTypes& other);
		MimeTypes& operator=(const MimeTypes& other);
		~MimeTypes();
};

#endif
