CC          =   clang++
CC_FLAGS    =   -Wall -Wextra -Werror
RM          =   rm -rf
DIR_HEADERS =   ../../incs/
SRC         =   Dispatcher.cpp \
				DispatcherCGI.cpp \
				DispatcherNegotiate.cpp \
				DispatcherStatusCode.cpp \
				DispatcherUtils.cpp
SRCS        =   $(SRC)
OBJS        =   $(SRCS:%.cpp=$(DIR_OBJS)%.o)
NAME        =   dispatcher

all:            $(NAME)
$(NAME):        $(OBJS)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.cpp
	$(CC) $(CC_FLAGS) -I $(DIR_HEADERS) -c $< -o $@

clean:
	@$(RM) $(OBJS)
fclean:         clean
	@$(RM) $(NAME)
re:             fclean all
.PHONY:         all clean fclean re bonus re_bonus
