bin_PROGRAMS = test_smaf

test_smaf_SOURCES = test_smaf.c
test_smaf_LDADD = ../lib/libsmaf.la

test_smaf_SOURCES += optee_invoke_sdp_ta.c optee_invoke_sdp_ta.h
test_smaf_LDADD += -lteec -L$(OPTEE_CLIENT_EXPORT)/lib
