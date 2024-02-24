# 智能指针(smart pointers)的使用

在这个sample中，我们使用更加现代的智能指针来管理C++内存。

在之前的练习中，我们都是手动使用`new`与`delete`关键字在堆(heap)中申请与解除内存块的使用权。一旦忘记使用`delete`，就会造成一个变量已经不再使用，但是其内存还未释放的情况（内存泄漏）。

在现代C++中，智能指针可以帮助我们自动跟踪变量的使用情况，在变量生存期结束时自动`delete`， 下面是一个对比：

```c++
void UseRawPointer()
{
    // Using a raw pointer -- not recommended.
    Song* pSong = new Song(L"Nothing on You", L"Bruno Mars"); 

    // Use pSong...

    // Don't forget to delete!
    delete pSong;   
}

void UseSmartPointer()
{
    // Declare a smart pointer on stack and pass it the raw pointer.
    unique_ptr<Song> song2(new Song(L"Nothing on You", L"Bruno Mars"));

    // Use song2...
    wstring s = song2->duration_;
    //...

} // song2 is deleted automatically here.
```

智能指针有三种类型(具体的区别与使用请阅读[这里](https://learn.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-170#c-standard-library-smart-pointers)): 
+ `unique_ptr`
+ `shared_ptr`
+ `weak_ptr`

相比于`1_BasicDArray`，使用了智能指针后，程序的前后对比：

1. Operator= 

    Old:
    ```C++
    // overload operator '='
    DArray& DArray::operator = (const DArray& arr) {
        delete[] m_pData;

        m_nSize = arr.m_nSize;
        m_pData = new double[m_nSize];

        for (int i = 0; i < m_nSize; i++)
            m_pData[i] = arr[i];

        return *this;
    }
    ```

    New:
    ```C++
    DArray& DArray::operator = (const DArray& arr) {

        m_nSize = arr.m_nSize;
        // Use reset to automatically release the original memory
        m_pData.reset(new double[m_nSize]);

        for (int i = 0; i < m_nSize; i++)
            m_pData[i] = arr[i];

        return *this;
    }
    ```

2. PushBack

   Old:
   ```C++
    void DArray::PushBack(double dValue) {
        double* pTemp = new double[static_cast<size_t>(m_nSize) + 1];

        for (int i = 0; i < m_nSize; i++)
            pTemp[i] = m_pData[i];

        pTemp[m_nSize] = dValue;

        delete[] m_pData;
        m_pData = pTemp;
        m_nSize++;
    }
   ```

   New:
   ```C++
    void DArray::PushBack(double dValue) {
	std::unique_ptr<double[]> pTemp(new double[static_cast<size_t>(m_nSize) + 1]);

	for (int i = 0; i < m_nSize; i++)
		pTemp[i] = m_pData[i];

	pTemp[m_nSize] = dValue;

	// The original array pointed by smart pointer m_pData will be automatically released
    // and pTemp will be nullptr
	m_pData = std::move(pTemp);
	m_nSize++;
    }
   ```


想了解更多关于智能指针的信息以及更多的好处，请阅读下面的参考资料。


## 参考资料
1. 详细的介绍：[GeeksForGeeks: smart pointers](https://www.geeksforgeeks.org/smart-pointers-cpp/)
2. 简洁的介绍：[智能指针（现代 C++）](https://learn.microsoft.com/zh-cn/cpp/cpp/smart-pointers-modern-cpp?view=msvc-170)
3. API手册：[cppreference: unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)