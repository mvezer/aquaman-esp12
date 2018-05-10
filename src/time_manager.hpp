#ifndef TIME_MANAGER_HPP
#define TIME_MANAGER_HPP

void tm_sync(uint32_t deltaTime);
int ts_parse_response(String json);
void tm_tick(uint32_t deltaTime);
void tm_date_str(char * str);
void tm_time_str(char * str);
void tm_display();
unsigned long tm_day_timestamp();
unsigned long tm_timestamp();

#endif
