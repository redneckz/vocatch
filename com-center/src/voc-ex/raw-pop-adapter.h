#ifndef RAWPOPADAPTER_
#define RAWPOPADAPTER_

#include <map>

#include "pop-element.h"

using namespace std;

namespace vocex {

/*
 * Адаптирует сырые данные прочитанные из pop/epop файла
 * и предоставляет данные о поправках по временным отметкам
 */
class RawPOPAdapter {

	map<unsigned long long, POPElement> els;

public:
	RawPOPAdapter() {
	}
	/*
	 * адаптация сырых данных
	 * epop - включить поддержку расширенного формата
	 */
	void adapt(unsigned char* rawData, unsigned long rawDataSize, bool epop) {
		els.clear();
		unsigned long long time = 0;
		while (rawDataSize > 0) {
			if (epop) {
				time = ((unsigned long long) rawData[0]) << 56
						| ((unsigned long long) rawData[1]) << 48
						| ((unsigned long long) rawData[2]) << 40
						| ((unsigned long long) rawData[3]) << 32
						| ((unsigned long long) rawData[4]) << 24
						| ((unsigned long long) rawData[5]) << 16
						| ((unsigned long long) rawData[6]) << 8
						| ((unsigned long long) rawData[7]);
				rawData += 8;
				rawDataSize -= 8;
			}
			unsigned short slideCount = ((unsigned short) rawData[0]) << 8
					| ((unsigned short) rawData[1]);
			rawData += 2;
			rawDataSize -= 2;
			for (long slideIndex = 0; slideIndex < slideCount; slideIndex++) {
				unsigned short angle = ((unsigned short) rawData[0]) << 8
						| ((unsigned short) rawData[1]);
				rawData += 2;
				rawDataSize -= 2;
				unsigned short offset = ((unsigned short) rawData[0]) << 8
						| ((unsigned short) rawData[1]);
				rawData += 2;
				rawDataSize -= 2;
				POPElement el(time, ((float) angle) / 60.0f, offset);
				els.insert(make_pair(time, el));
				time += 80;
			}
		}
	}
	/*
	 * реальные элементы
	 */
	map<unsigned long long, POPElement> getRealPOPElementsMap() {
		return els;
	}
	/*
	 * первая временная метка
	 */
	unsigned long long getStartTime() {
		return els.begin()->first;
	}
	/*
	 * последняя временная метка
	 */
	unsigned long long getEndTime() {
		return (--els.end())->first;
	}
	/*
	 * линейное приближение
	 * time - временная метка в миллисекундах
	 */
	POPElement getPOPElement(unsigned long long time) {
		if (els.empty()) {
			throw "POP Data is empty!";
		}
		map<unsigned long long, POPElement>::iterator rightI = els.lower_bound(
				time);
		if (rightI == els.end()) {
			return (--rightI)->second;
		}
		if (rightI == els.begin()) {
			return rightI->second;
		}
		if (rightI->first == time) {
			return rightI->second;
		}
		map<unsigned long long, POPElement>::iterator leftI = rightI;
		leftI--;
		unsigned long long lTime = leftI->first;
		unsigned long long uTime = rightI->first;
		unsigned long long dTime = uTime - lTime;
		float lAngle = leftI->second.getAngle();
		float uAngle = rightI->second.getAngle();
		float angle = lAngle + (uAngle - lAngle) * (time - lTime) / dTime;
		unsigned short lOffset = leftI->second.getOffset();
		unsigned short uOffset = rightI->second.getOffset();
		unsigned short offset = lOffset
				+ (uOffset - lOffset) * (time - lTime) / dTime;
		POPElement result(time, angle, offset);
		return result;
	}

};

}

#endif /*RAWPOPADAPTER_*/
