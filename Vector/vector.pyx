# cython: language_level=3

from libc.stdlib cimport calloc, free

import sys

cdef WriteErr(str message):
    sys.stderr.write(f"{message}")
    sys.exit(1)

ctypedef signed int int32_t
ctypedef signed long long int64_t

cdef extern from "./include/vector_int.h" nogil:
    ctypedef struct VectorInt_t:
        int32_t *ptr
        size_t size
        size_t capacity

    VectorInt_t* vector_int_init(size_t capacity) nogil
    VectorInt_t* vector_int_from_array(int32_t *arr, size_t n) nogil
    void vector_int_free(VectorInt_t *v) nogil

    void vector_int_pushback(VectorInt_t *v, int32_t val) nogil
    void vector_int_extend(VectorInt_t *v, int32_t *src, size_t n) nogil
    void vector_int_emplaceback(VectorInt_t *v, int32_t val) nogil
    void vector_int_emplace(VectorInt_t *v, int32_t *src, size_t n) nogil
    void vector_int_copy(VectorInt_t *v0, VectorInt_t *v1) nogil

    void vector_int_sort(VectorInt_t *v) nogil
    int32_t vector_int_find(VectorInt_t *v, int32_t key) nogil
    int32_t vector_int_count(VectorInt_t *v, int32_t key) nogil
    VectorInt_t* vector_int_findall(VectorInt_t *v, int32_t key) nogil

    void vector_int_iadd(VectorInt_t *v0, VectorInt_t *v1) nogil
    VectorInt_t* vector_int_add(VectorInt_t *v0, VectorInt_t *v1) nogil

    void vector_int_isub(VectorInt_t *v0, VectorInt_t *v1) nogil
    VectorInt_t* vector_int_sub(VectorInt_t *v0, VectorInt_t *v1) nogil

    void vector_int_imul(VectorInt_t *v0, VectorInt_t *v1) nogil
    VectorInt_t* vector_int_mul(VectorInt_t *v0, VectorInt_t *v1) nogil

    int64_t vector_int_dotproduct(VectorInt_t *v0, VectorInt_t *v1) nogil
    int64_t vector_int_sum(VectorInt_t *v) nogil
    void vector_int_axpy(VectorInt_t *v0, VectorInt_t *v1, int32_t a) nogil

import cython

@cython.boundscheck(False)
@cython.wraparound(False)
cdef class VectorInt:
    __slots__ = ("_cvec",)

    cdef VectorInt_t* _cvec

    def __cinit__(self, capacity_or_list):
        if isinstance(capacity_or_list, int):
            self._cvec = vector_int_init(capacity_or_list)
        else:
            # Nếu truyền vào list, cứ khởi tạo 0 hoặc size list 
            # rồi mới nạp dữ liệu vào sau
            self._cvec = vector_int_init(len(capacity_or_list))

    def __init__(self, capacity_or_list):
        # __init__ chạy SAU __cinit__
        if isinstance(capacity_or_list, list):
            self.extendls(capacity_or_list)

    @classmethod
    def from_array(cls, int32_t[:] arr):
        cdef VectorInt v = cls(arr.shape[0]) # Khởi tạo wrapper
        with nogil: 
            vector_int_emplace(v._cvec, &arr[0], arr.shape[0]) # Nạp dữ liệu
        return v
    
    @classmethod
    def from_list(cls, list ls):
        cdef size_t size = len(ls)
        cdef VectorInt v = cls(size)
        cdef int32_t* ptr = <int32_t*>v._cvec.ptr
        cdef int i
        for i in range(size):
            ptr[i] = ls[i]
        v._cvec.size = size
        return v
    
    cpdef push_back(self, int32_t val):
        with nogil: vector_int_pushback(self._cvec, val)
        return self
    
    cpdef extendls(self, list ls):
        cdef size_t n = len(ls)
        if n <= 0:
            WriteErr("Exception: List size mismatch.")
        # Tạm cấp phát một buffer C để chuyển đổi
        cdef int32_t* temp_buf = <int32_t*>calloc(n, 4)
        
        # Copy từ list vào buffer C
        cdef int i
        for i in range(n):
            temp_buf[i] = ls[i]
            
        # Gọi hàm C-Core của bạn
        vector_int_extend(self._cvec, temp_buf, n)
        
        # Giải phóng buffer tạm
        free(temp_buf)
    
    cpdef extendarr(self, int32_t[:] arr):
        cdef size_t size = arr.shape[0]
        if size <= 0:
            WriteErr("Exception: Array size mismatch.")
        with nogil:
            vector_int_extend(self._cvec, &arr[0], size)
    
    cpdef emplace_back(self, int32_t val):
        vector_int_emplaceback(self._cvec, val)
        return self
    
    cpdef emplace(self, int32_t[:] arr):
        vector_int_emplace(self._cvec, &arr[0], arr.shape[0])
        return self
    
    cpdef copy(self, VectorInt other) :
        vector_int_copy(self._cvec, other._cvec)

    cpdef dot_product(self, VectorInt other):
        cdef int64_t total = vector_int_dotproduct(self._cvec, other._cvec)

        return total

    cpdef axpy(self, VectorInt y, int32_t a) :
        vector_int_axpy(self._cvec, y._cvec, a)

    cpdef sort(self) :
        vector_int_sort(self._cvec)
    
    cpdef find(self, int32_t key) :
        return vector_int_find(self._cvec, key)

    cpdef count(self, int32_t key):
        return vector_int_count(self._cvec, key)

    cpdef findall(self, int32_t key):
        cdef VectorInt_t *p = vector_int_findall(self._cvec, key)
        cdef VectorInt v = VectorInt(0)
        v._cvec = p
        return v

    cpdef int64_t sum(self):
        return vector_int_sum(self._cvec)

    def __len__(self):
        return self._cvec.size

    def __getitem__(self, Py_ssize_t i):
        # Hỗ trợ index âm theo đúng chuẩn Python
        if i < 0:
            i += self._cvec.size
        # Bỏ qua check biên nếu bạn thực sự tự tin về dữ liệu để tăng tốc
        if i >= self._cvec.size or i < 0:
            WriteErr("Index out of range.")
        return self._cvec.ptr[i]

    def __setitem__(self, Py_ssize_t i, int32_t value):
        if i < 0:
            i += self._cvec.size
        # Bỏ qua check biên nếu bạn thực sự tự tin về dữ liệu để tăng tốc
        if i >= self._cvec.size or i < 0:
            WriteErr("Index out of range.")
        self._cvec.ptr[i] = value

    
    def __iter__(self):
        # Biến con trỏ thành memoryview để Python tự sinh iterator bằng C siêu nhanh
        return iter(<int32_t[:self._cvec.size]> self._cvec.ptr)

    def __iadd__(self, VectorInt other):
        vector_int_iadd(self._cvec, other._cvec)
        return self
    
    def __add__(self, VectorInt other):
        # 1. Gọi hàm C trả về con trỏ mới
        cdef VectorInt_t* raw_ptr = vector_int_add(self._cvec, other._cvec)
        if raw_ptr == NULL:
            WriteErr("MemoryError.")
        
        # 2. Tạo một đối tượng VectorInt "rỗng"
        cdef VectorInt res = VectorInt.__new__(VectorInt)
        
        # 3. Gán con trỏ đó cho res
        res._cvec = NULL
        res._cvec = raw_ptr
        
        return res
    
    def __isub__(self, VectorInt other):
        vector_int_isub(self._cvec, other._cvec)
        return self
    
    def __sub__(self, VectorInt other):
        cdef VectorInt_t* raw_ptr = vector_int_sub(self._cvec, other._cvec)
        if raw_ptr == NULL:
            WriteErr("MemoryError.")

        cdef VectorInt res = VectorInt.__new__(VectorInt)
        res._cvec = NULL
        res._cvec = raw_ptr

        return res
    
    def __imul__(self, VectorInt other):
        vector_int_imul(self._cvec, other._cvec)
        return self
    
    def __mul__(self, VectorInt other):
        cdef VectorInt_t* raw_ptr = vector_int_mul(self._cvec, other._cvec)
        if raw_ptr == NULL:
            WriteErr("MemoryError.")

        cdef VectorInt res = VectorInt.__new__(VectorInt)
        res._cvec = NULL
        res._cvec = raw_ptr

        return res
    
    def __matmul__(self, VectorInt other):
        return vector_int_dotproduct(self._cvec, other._cvec)


    def __repr__(self):
        cdef size_t n = self._cvec.size
        if n == 0: return "vector<int>([] )"
        
        # Định dạng theo phong cách khoa học: [ x, y, z ... ]
        # Nếu ít thì in hết, nếu nhiều thì in kiểu rút gọn
        if n <= 10:
            elements = [str(self._cvec.ptr[i]) for i in range(n)]
            return f"vector<int>([{' '.join(elements)}])"
        else:
            # Style chuyên nghiệp: 5 đầu, 5 cuối
            head = [str(self._cvec.ptr[i]) for i in range(5)]
            tail = [str(self._cvec.ptr[n-5+i]) for i in range(5)]
            return f"vector<int>([{' '.join(head)}, ..., {' '.join(tail)}]) (size={n})"

    def __dealloc__(self):
        if self._cvec != NULL:
            vector_int_free(self._cvec)
            self._cvec = NULL