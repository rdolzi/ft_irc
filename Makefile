NAME = ircserv
SRC_DIR = ./src
OBJ_DIR = ./obj
INCLUDE_DIR = ./include

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(SRC_DIR)/client/*.cpp) \
       $(wildcard $(SRC_DIR)/server/*.cpp) \
	   $(wildcard $(SRC_DIR)/server/command/*.cpp) \
	   $(wildcard $(SRC_DIR)/server/channel/*.cpp) \
	   $(wildcard $(SRC_DIR)/logger/*.cpp) 

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Compiler and flags
CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDE = -I$(INCLUDE_DIR) -I$(SRC_DIR)

# Colors and formatting
GREEN = \033[32m
BLUE = \033[34m
CYAN = \033[36m
BOLD = \033[1m
UNDER = \033[4m
END = \033[0m
RESET = \033[0;0m

# Build variables
SRC_COUNT = 0
SRC_COUNT_TOT := $(words $(SRCS))


# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(FLAGS) $(INCLUDE) -c $< -o $@
	@$(eval SRC_COUNT = $(shell expr $(SRC_COUNT) + 1))
	@printf "\r$(BOLD)$(GREEN)[%d/%d] ⏳(%d%%)$(END)$(BOLD)$(BLUE)%-40s" $(SRC_COUNT) $(SRC_COUNT_TOT) $(shell expr 100 \* $(SRC_COUNT) / $(SRC_COUNT_TOT)) $<


# Main target
$(NAME): $(OBJS)
	@$(CXX) $(OBJS) -o $(NAME)
	@printf "\n\n"
	@printf "$(UNDER)$(BOLD)$(CYAN)IRC Server Compiled Successfully!$(RESET)\n"
	@printf "\n"
	@printf "$(BOLD)$(GREEN)  _____ _____   _____ \n"
	@printf " |_   _|  __ \\ / ____|   \n"
	@printf "   | | | |__) | |         \n"
	@printf "   | | |  _  /| |         \n"
	@printf "  _| |_| | \\ \\| |____   \n"
	@printf " |_____|_|  \\_\\\\_____| \n"
	@printf "                          \n"
	@printf " Internet Relay Chat         $(RESET)\n"
	@printf " $(UNDER)$(BOLD)$(GREEN)Made by:\n$(RESET) 	   $(UNDER)$(BOLD)$(GREEN)Fcardina and Rdolzi\n$(RESET)"
	@printf "\n"
	@printf "$(UNDER)$(BOLD)$(CYAN)Usage: ./$(NAME) <port> <password>$(RESET)\n"

all: $(NAME)

clean:
	${RM} ${OBJS}
	${RM} -r ${OBJ_DIR}
	@printf "$(BOLD)$(BLUE)Cleaned object files$(RESET)\n"

fclean: clean
	${RM} ${NAME}
	@printf "$(BOLD)$(BLUE)Cleaned executable$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re