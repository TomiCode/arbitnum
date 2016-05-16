CR = \033[0m
CCODE = \033[1;34m
COBJ = \033[1;33m
CCLEAR = \033[1;31m

CC = clang++
CFLAGS := $(CFLAGS) -g -W -Werror -Wextra -D__DEBUG_LOG
LIBS = 

ELF = ArbitraryNumbers
SRCS = Number.cc main.cc

OBJS = ${SRCS:.cc=.o}

.PHONY: clean distclean
$(ELF): $(OBJS)
	@printf "$(COBJ)[LD]$(CR) $<\n"
	@$(CC) $(CFLAGS) $(OBJS) -o $(ELF) $(LIBS)

%.o: %.cc $(SRCS)
	@printf "$(CCODE)[CC]$(CR) $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	@for RF in $(OBJS) ; do \
		echo -e "$(CCLEAR)[RM]$(CR) $$RF"; \
		rm -rf $$RF; \
	done

distclean:
	@echo -e "$(CCLEAR)[RM]$(CR) $(ELF)"
	@rm -rf $(ELF)
