
#define DM_MSG(str) MessageBox((LPCTSTR)(CString(str)));

#define DM_TEST_TIME

#ifdef DM_TEST_TIME
#define DM_TEST_TIME_ST clock_t start, finish;\
	start = clock();

#define DM_TEST_TIME_ED 	finish = clock();\
	double iMs = (finish - start) * 1000.0 / CLOCKS_PER_SEC;\
	CString str;\
	str.Format(_T("Time cost: %.2f ms"), iMs);\
	DM_MSG(str);
#else
#define DM_TEST_TIME_ST 
#define DM_TEST_TIME_ED 
#endif

class ComplexNum
{
public:
	ComplexNum():
	  imag(0.0F), real(0.0F){
	};

	~ComplexNum(){
	}
public:
	double imag;
	double real;
};
