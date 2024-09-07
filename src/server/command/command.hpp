/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:40:13 by rdolzi            #+#    #+#             */
/*   Updated: 2024/09/07 20:40:14 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

class Command {
private:
    std::string _prefix;
    std::string _command;
    std::vector<std::string> _parameters;

public:
    Command();
    Command(const std::string& prefix, const std::string& command, const std::vector<std::string>& parameters);

    // Getters
    std::string getPrefix() const;
    std::string getCommand() const;
    std::vector<std::string> getParameters() const;

    // Setters
    void setPrefix(const std::string& prefix);
    void setCommand(const std::string& command);
    void addParameter(const std::string& parameter);

    // Utility methods
    std::string toString() const;
    bool isValid() const;
};

#endif // COMMAND_HPP