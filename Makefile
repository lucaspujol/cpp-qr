CXX			=	g++
CXXFLAGS	=	-std=c++17 -Wall -Wextra
TESTFLAGS	=	-g3 -lcriterion

SRCS		=	$(shell find src -name "*.cpp" ! -name "main.cpp")
TEST_SRCS	=	$(shell find tests -name "*.cpp")
OBJDIR		=	obj
OBJS		=	$(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS))
TEST_OBJS	=	$(patsubst tests/%.cpp,$(OBJDIR)/tests/%.o,$(TEST_SRCS))
MAIN		=	src/main.cpp

NAME		=	cppqr
TEST_NAME	=	test_cppqr
EXEC		=	$(NAME).out
TEST_EXEC	=	$(TEST_NAME).out

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(MAIN) -o $@ $^

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/tests/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f $(TEST_OBJS)

fclean: clean
	rm -f $(EXEC)
	rm -f tests/$(TEST_EXEC)

re: fclean all

test: $(TEST_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(TESTFLAGS) -o tests/$(TEST_EXEC)

run:
	@./$(EXEC)

.PHONY: all clean fclean re run test
