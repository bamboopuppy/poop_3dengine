#include "stdafx.h"
#include "tools.h"

//#define MAX_EVENTS 50

/*struct t_event {
	void (*function) (void *);
	unsigned int time_to_run;
	void * data;
	int position;
};*/

std::vector<t_event*> events;

void clear_events()
{
	/*int i;
	for (i = 0; i < MAX_EVENTS; i++){
		if(events[i] != NULL){
			destroy_event(i);
		}

		events[i] = NULL;
	}*/
	unsigned int i;
	for (i = 0; i < events.size(); i++)
	{
		if (events[i]!= NULL)
			destroy_event(events[i]);
			//free(events[i]);
	}
	events.clear();
}

void add_event(void (*function) (struct t_event *), int time_to_run, void * data)
{
	struct t_event * new_event = (struct t_event *) malloc(sizeof(struct t_event));
	new_event->function = function;
	new_event->time_to_run = time_to_run;
	new_event->data = data;
	//new_event->position = position;
	new_event->destroy = false;
	events.push_back(new_event);
}

void destroy_event(struct t_event * event2delete)
{
	unsigned int i;
	for (i = 0; i < events.size(); i++)
	{
		if (event2delete != NULL && (void *)events[i]  == (void *) event2delete) {
			event2delete->destroy = true;
			event2delete->function(event2delete);
			free(event2delete);
			events.erase(events.begin() + i);
			break;
		}
	}
}

void check_events_due(unsigned int cur_time)
{
	unsigned int i;
	for (i = 0; i < events.size(); i++)
	{
		if (events[i] == NULL)
			continue;
		if (events[i]->time_to_run <= cur_time)
			events[i]->function(events[i]);
	}
}
