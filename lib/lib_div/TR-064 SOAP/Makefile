#!/bin/bash

EXAMPLEFILE := arduino_secrets.h.example
EXAMPLS_DIR := ./examples
CALLER := $(EXAMPLS_DIR)/caller
HOME := $(EXAMPLS_DIR)/home-indicator

all: home-indicator caller 
.PHONY:  all

caller:$(CALLER)
	# If $(EXAMPLEFILE) exists, move it to arduino_secrets.h
	@echo "Find in " $</$(EXAMPLEFILE)
	@if [ -f $</$(EXAMPLEFILE) ]; then \
		mv $</$(EXAMPLEFILE) $</arduino_secrets.h; \
	else echo "Not found"; \
	fi

home-indicator:$(HOME)
	# If $(EXAMPLEFILE) exists, move it to arduino_secrets.h
	@echo "Find in " $</$(EXAMPLEFILE)
	@if [ -f $</$(EXAMPLEFILE) ]; then \
		mv $</$(EXAMPLEFILE) $</arduino_secrets.h; \
	else echo "Not found"; \
	fi
