#ifndef HEADLESS_PARSER_H
#define HEADLESS_PARSER_H

typedef enum HeadlessActionType {
    HEADLESS_ACTION_TYPE_GRBL = 0,
    HEADLESS_ACTION_TYPE_SETTINGS_SET = 1,
    HEADLESS_ACTION_TYPE_SETTINGS_GET = 2,
    HEADLESS_ACTION_TYPE_STATUS = 3,
} HeadlessActionType;

void headless_process_line(char *line);

#endif
