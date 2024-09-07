/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:39:08 by rdolzi            #+#    #+#             */
/*   Updated: 2024/09/07 20:39:51 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include "command.hpp"
#include "../../logger/logger.hpp"
#include "../../utils/server_utils.hpp"
#include <sstream>
#include <algorithm>
#include <string>

class CommandParser {
public:
    static Command parse(const std::string& message);

private:
    static std::string extractPrefix(std::string& message);
    static std::string extractCommand(std::string& message);
    static std::vector<std::string> extractParameters(std::string& message);
};

#endif // COMMANDPARSER_HPP