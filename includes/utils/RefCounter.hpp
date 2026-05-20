/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RefCounter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 15:02:10 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/16 15:14:53 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REFCOUNTER_HPP
#define REFCOUNTER_HPP

class RefCounter {
	private:
		int _refCount;

	protected:
		/// @brief Virtual destructor to ensure the derived class destructor
		/// does its job safely
		virtual ~RefCounter();

	public:
		RefCounter();
		RefCounter(const RefCounter& other);
		RefCounter& operator=(const RefCounter& other);

		void add_ref();
		void release();
};
#endif
