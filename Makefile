CXX			=	g++
CXXFLAGS	=	-std=c++17 -Wall -Wextra
SRCS		=	$(shell find . -name "*.cpp")
OBJDIR		=	obj
OBJS		=	$(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
NAME		=	cppqr
EXEC		=	$(NAME).out

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(EXEC)

re: fclean all

run:
	@make -sj all
	@make -s clean
	@./$(EXEC)

.PHONY: all clean fclean re run
