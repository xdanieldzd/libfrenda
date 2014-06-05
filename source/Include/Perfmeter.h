/*------------------------------------------------------------
	INCLUDE/PERFMETER.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_PERFMETER_H
#define _INCLUDED_PERFMETER_H

namespace frenda
{
	class Perfmeter
	{
		public:
			static const float barLength;
			bool enabled;
			
			Perfmeter(frenda::System *);
			void render();
			
		private:
			frenda::System *sys;
			void renderBar(float, float, float, float, uint32, uint32);
	};
}

#endif
