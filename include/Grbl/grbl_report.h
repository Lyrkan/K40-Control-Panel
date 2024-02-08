#ifndef GRBL_REPORT_H
#define GRBL_REPORT_H

#include "Grbl/grbl_state.h"

class GrblReport {
  public:
    GrblState state;
    GrblAlarm alarm;
    GrblCoord w_pos;
    GrblCoord m_pos;
    GrblCoord wco;
    GrblBufferState buffer_state;
    GrblFeedState feed_state;
    int line_number;
    int active_pins;

    GrblReport();
    void update(const GrblReport *report);
};

extern GrblReport grbl_last_report;
void grbl_update_last_report(const GrblReport *report);
void grbl_update_last_alarm(GrblAlarm alarm);

#endif
