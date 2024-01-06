#ifndef ARGS_H_
#define ARGS_H_

typedef enum e_argpres {
    ArgParseResult_OK,
    ArgParseResult_UnknownArgument,
    ArgParseResult_InvalidArgument,
    ArgParseResult_AllocationError,
    ArgParseResult_HelpCommand
} ArgParseResult;

ArgParseResult argsParse(int argc, char **argv);
int argsGetScreenWidth(void);
int argsGetScreenHeight(void);
char *argsGetLevelName(void);
void argsCleanup(void);

#endif
