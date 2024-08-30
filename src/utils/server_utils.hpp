/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_utils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcardina <fcardina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 16:59:49 by fcardina          #+#    #+#             */
/*   Updated: 2024/08/29 17:03:22 by fcardina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_UTILS_HPP
#define SERVER_UTILS_HPP

#include <sstream>
#include <string>

template <typename T>
std::string to_string(T value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

#endif 
