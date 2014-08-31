#include <windows.h>

#include <iostream>

#include <map>
#include <vector>
#include <string>

/*
 * подключение классов voc-ex
 */
#include "./voc-ex/voc-ex.h"

using namespace std;
/*
 * подключение пространства имён voc-ex
 */
using namespace vocex;

int main(int argc, const char* argv[]) {
	/*
	 * объект для удалённого управления КУП
	 */
	ComCenter c(127, 0, 0, 1);
	cout << "Sending prepare..." << endl;
	/*
	 * команда подготовки КУП (посылать один раз)
	 */
	c.sendPrepare();
	cout << "Sending start..." << endl;
	/*
	 * <<<вычисление временной метки посылки команды пуска
	 */
	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);
	unsigned long long startTime = (unsigned long long) systemTime.wSecond
			* 1000 + systemTime.wMilliseconds;
	/*
	 * вычисление временной метки посылки команды пуска>>>
	 */
	/*
	 * команда старта КУП
	 */
	c.sendStart();
	/*
	 * какая-то работа
	 */
	Sleep(1000);

	cout << "Sending stop..." << endl;
	/*
	 * буфер для хранения данных из файла
	 */
	vector<unsigned char> rawPOPData;
	/*
	 * строка для хранения имени варианта
	 */
	string variant;
	/*
	 * команда остановки КУП
	 */
	c.sendStop(rawPOPData, variant);
	if (rawPOPData.size() > 0) {
		/*
		 *  указатель на сырые данные прочитанные из файла
		 */
		vector<unsigned char>::pointer rawPOPDataPointer = &rawPOPData.front();
		RawPOPAdapter adapter;
		/*
		 *  адаптация
		 */
		adapter.adapt(rawPOPDataPointer, rawPOPData.size(), true);
		/*
		 *  вывод временной метки посылки команды пуска и
		 *  имени текущего варианта
		 */
		cout << "Start command send time-point:" << startTime << endl;
		cout << "Variant: " << variant << endl;
		/*
		 *  <<<вывод всех реальных записей в файле поправок
		 */
		map<unsigned long long, POPElement> els =
				adapter.getRealPOPElementsMap();
		map<unsigned long long, POPElement>::iterator elsI = els.begin();
		for (; elsI != els.end(); elsI++) {
			POPElement& el = elsI->second;
			cout << el.toString() << endl;
		}
		/*
		 *  вывод всех реальных записей в файле поправок>>>
		 */
		cout << "---------------------------" << endl;
		/*
		 *  вывод первой и последней временных меток
		 */
		cout << adapter.getStartTime() << '-' << adapter.getEndTime() << endl;
		/*
		 *  вывод интерполированной поправки
		 */
		cout << "---------------------------" << endl;
		POPElement el = adapter.getPOPElement(adapter.getStartTime() + 40);
		cout << el.toString() << endl;
	} else {
		cerr << "Invalid file format!" << endl;
	}
	return 0;
}

