#ifndef POPELEMENT_H_
#define POPELEMENT_H_

#include <string>
#include <sstream>

using namespace std;

namespace vocex {

/*
 * Инкапсулирует данные о поправке и смещениии
 */
class POPElement {

	unsigned long long time;
	float angle;
	unsigned short offset;

public:
	POPElement() {
		time = 0;
		angle = 0.0f;
		offset = 0;
	}
	POPElement(unsigned long long time, float angle, unsigned short offset) {
		this->time = time;
		this->angle = angle;
		this->offset = offset;
	}
	/*
	 * временная метка в миллисекундах 
	 */
	unsigned long long getTime() {
		return time;
	}
	/*
	 * поправка в градусах 
	 */
	float getAngle() {
		return angle;
	}
	/*
	 * смещение в пикселах 
	 */
	unsigned short getOffset() {
		return offset;
	}

	string toString() {
		stringstream buf;
		buf << "POPElement{time=" << time << ";angle=" << angle << ";offset="
				<< offset << '}';
		buf.flush();
		return buf.str();
	}

};

}

#endif /*POPELEMENT_H_*/
