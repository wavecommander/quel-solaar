#include "stdlib.h"
#include "forge.h"
#include "imagine.h"



IIntelligence	*i_intelligence_create(uint om_max,	uint em_max, float *needs)
{
	IIntelligence *intelligence;
	uint i;
	intelligence = malloc(sizeof *intelligence);
	intelligence->om = malloc((sizeof *intelligence->om) * om_max);
	intelligence->om_count = 0;
	intelligence->om_max = om_max;
	intelligence->em = malloc((sizeof *intelligence->em) * em_max);
	intelligence->em_count = 0;
	intelligence->em_max = em_max;
	for(i = 0; i < IMAGINE_NEEDS_COUNT; i++)
		intelligence->needs[i] = needs[i];
	return intelligence;
}

void i_intelligence_memory_add(IEventMemory *event)
{
/*	uint slot;
	if(intelligence->om_count == intelligence->om_max)
	{
		slot = 0;
	}else
		slot = intelligence->om_count++:
	intelligence->om[slot] = *event;

*/


}

void			i_memory_evaluate(IIntelligence *i);