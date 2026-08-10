#include "../include/vector_int.h"
// Setup Object
VectorInt_t* vector_int_init(size_t capacity) {
    VectorInt_t *v = (VectorInt_t*)calloc(1, sizeof(VectorInt_t));
    if (!v) return NULL;

    v->ptr = (int32_t*)calloc(capacity, 4);
    if (!v->ptr) {
        free(v);
        return NULL;
    }
    
    v->size = 0;
    v->capacity = capacity;
    return v;
}

VectorInt_t* vector_int_from_array(int32_t *restrict arr, size_t n) {
    VectorInt_t *v = vector_int_init(n);
    if (!v) return NULL;
    memcpy(v->ptr, arr, n << 2);
    v->size = n;
    return v;
}

void vector_int_free(VectorInt_t *restrict v) {
    if (v) {
        if (v->ptr) {
            free(v->ptr);
            v->ptr = NULL;
        }
        free(v);
    }
}


// Logic Obj


void vector_int_pushback(VectorInt_t *restrict v, int32_t val) {
    if (v->size >= v->capacity) {
        size_t new_cap = v->capacity << 1; // Chiến thuật tăng gấp đôi
        int32_t *new_ptr = (int32_t*)realloc(v->ptr, new_cap << 2);
        if (!new_ptr) return; 
        
        v->ptr = new_ptr;
        v->capacity = new_cap;
    }
    v->ptr[v->size++] = val;
}

void vector_int_extend(VectorInt_t *restrict v, int32_t *restrict src, size_t n) {
    if (v->size + n > v->capacity) {
        size_t new_cap = v->capacity + n; 
        int32_t *new_ptr = realloc(v->ptr, new_cap << 2);
        if (!new_ptr) return;
        v->ptr = new_ptr;
        v->capacity = new_cap;
    }
    memcpy(v->ptr + v->size, src, n << 2);
    v->size += n;
}

void vector_int_reserve(VectorInt_t *restrict v, size_t new_capacity) {
    if (new_capacity <= v->capacity) return;
    int32_t *new_ptr = (int32_t*)realloc(v->ptr, new_capacity << 2);
    if (!new_ptr) return;
    size_t diff = new_capacity - v->capacity;
    memset(new_ptr + v->capacity, 0, diff << 2);

    v->ptr = new_ptr;
    v->capacity = new_capacity;
}

void vector_int_emplaceback(VectorInt_t *restrict v, int32_t val) {
    v->ptr[v->size++] = val;
}

void vector_int_emplace(VectorInt_t *restrict v, int32_t *restrict src, size_t n) {
    memcpy(v->ptr + v->size, src, n << 2);
    v->size += n;
}

void vector_int_copy(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    if (v0->size < v1->size) {
        vector_int_reserve(v0, v1->size);
    }

    memcpy(v0->ptr, v1->ptr, v1->size << 2);
    v0->size = v1->size;
}

// RADIX SORT
void vector_int_sort(VectorInt_t *v) {
    size_t size = v->size;
    if (size <= 1) return; // Bảo vệ an toàn nếu mảng rỗng hoặc có 1 phần tử

    // Giữ lại con trỏ gốc được cấp phát để free chính xác ở cuối hàm
    uint32_t *allocated_dst = (uint32_t*)malloc(size * sizeof(uint32_t));
    
    uint32_t *src = (uint32_t*)v->ptr;
    uint32_t *dst = allocated_dst;
    uint32_t *tmp;
    
    int shift = 0;
    while (shift < 32) {
        size_t count[256] = {0};
        
        uint32_t *ptr = src;
        uint32_t *src_end = src + size; // Đổi tên thành src_end để không bị trùng biến
        
        // 1. Bước Đếm (Count) - Branchless
        // Phép XOR bit dấu 0x80 ở lượt dịch cuối (shift == 24) để xử lý đúng số int32_t có dấu
        uint32_t sign_flip = (uint32_t)(shift == 24) << 7; 
        while (ptr < src_end) {
            uint32_t bucket = ((*ptr++ >> shift) & 0xFF) ^ sign_flip;
            count[bucket]++;
        }
        
        // 2. Bước Prefix sum
        size_t pos[256];
        size_t *p_pos = pos;
        size_t *p_count = count;
        size_t *pos_end = pos + 255; // Đổi tên thành pos_end để tránh trùng biến
        size_t accumulator = 0;

        *p_pos = 0;
        while (p_pos < pos_end) {
            accumulator += *p_count++;
            *++p_pos = accumulator;
        }
        
        // 3. Bước Phân phối (Distribution) - Branchless
        ptr = src; // Reset lại ptr về đầu mảng dữ liệu src
        while (ptr < src_end) {
            uint32_t bucket = ((*ptr >> shift) & 0xFF) ^ sign_flip;
            dst[pos[bucket]++] = *ptr++;
        }
        
        // 4. Swap con trỏ qua từng lượt dịch 8-bit
        tmp = src;
        src = dst;
        dst = tmp;
        
        shift += 8;
    }
    
    // Copy ngược lại nếu sau 4 lượt, con trỏ src kết quả đang bị lệch khỏi v->ptr
    // Khử rẽ nhánh if (src != v->ptr) bằng toán toán học/memcpy kích thước động
    size_t bytes_to_copy = size * sizeof(uint32_t) * (src != (uint32_t*)v->ptr);
    memcpy(v->ptr, src, bytes_to_copy);
    
    // Giải phóng chính xác vùng nhớ đã malloc ban đầu, không lo bị hoán đổi con trỏ
    free(allocated_dst);
}



int32_t vector_int_find(VectorInt_t *v, int32_t key) {
    if (v->size == 0) return -1;

    size_t low = 0, high = v->size - 1;
    int steps = 0;
    int max_steps = 10; // Giới hạn số lần thử nội suy

    while (low <= high && key >= v->ptr[low] && key <= v->ptr[high]) {
        // Nếu nội suy thất bại trong việc thu hẹp phạm vi, dùng Binary Search
        if (steps++ > max_steps || v->ptr[high] == v->ptr[low]) {
            size_t mid = low + ((high - low) >> 1);
            if (v->ptr[mid] == key) return (int32_t)mid;
            if (v->ptr[mid] < key) low = mid + 1;
            else high = mid - 1;
        } else {
            // Logic nội suy
            size_t pos = low + (size_t)(((int64_t)(key - v->ptr[low]) * (high - low)) / 
                                        (v->ptr[high] - v->ptr[low]));
            if (v->ptr[pos] == key) return (int32_t)pos;
            if (v->ptr[pos] < key) low = pos + 1;
            else high = pos - 1;
        }
    }
    return -1;
}

// Tìm vị trí xuất hiện đầu tiên
int32_t lower_bound(VectorInt_t *v, int32_t key) {
    size_t low = 0, high = v->size;
    while (low < high) {
        size_t mid = low + ((high - low) >> 1);
        if (v->ptr[mid] < key) low = mid + 1;
        else high = mid;
    }
    return (int32_t)low;
}

// Tìm vị trí xuất hiện sau phần tử cuối cùng của key
int32_t upper_bound(VectorInt_t *v, int32_t key) {
    size_t low = 0, high = v->size;
    while (low < high) {
        size_t mid = low + ((high - low) >> 1);
        if (v->ptr[mid] <= key) low = mid + 1;
        else high = mid;
    }
    return (int32_t)low;
}

int32_t vector_int_count(VectorInt_t *v, int32_t key) {
    int32_t lb = lower_bound(v, key);
    int32_t ub = upper_bound(v, key);

    int32_t count = ub - lb;
    return count;
}

VectorInt_t* vector_int_findall(VectorInt_t *v, int32_t key) {
    int32_t start = lower_bound(v, key);
    int32_t end = upper_bound(v, key);
    
    // Tạo vector mới chứa các index
    VectorInt_t *results = vector_int_init(end - start);
    int32_t *d = results->ptr;
    while (start < end) {
        *d++ = start++;
    }
    return results;
}

void vector_int_clear(VectorInt_t *restrict v) {
    size_t size = v->size;
    int32_t *ptr = v->ptr;
    int32_t *end = ptr + size;
    while (ptr < end) {
        *ptr++ = 0;
    }
}

// Obj Function for Py

void vector_int_iadd(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));

    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *end = p0 + min_size;
    while (p0 < end) {
        *p0++ += *p1++;
    }
}

VectorInt_t* vector_int_add(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    size_t m = v0->size;
    size_t n = v1->size;

    size_t min_size = n ^ ((m ^ n) & -(m < n));
    size_t max_size = m ^ ((m ^ n) & -(m < n));
    
    VectorInt_t *v_res = vector_int_init(max_size);
    if (!v_res) return NULL;
    
    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *pr = v_res->ptr;
    int32_t *end = pr + min_size;
    
    while (pr < end) {
        *pr++ = *p0++ + *p1++;
    }

    size_t mask = -(size_t)(v0->size > v1->size);
    int32_t *src_remaining = (int32_t*)(((uintptr_t)(v0->ptr + min_size) & mask) | ((uintptr_t)(v1->ptr + min_size) & ~mask));
    size_t remaining_len = max_size - min_size;
    memcpy(pr + min_size, src_remaining, remaining_len << 2);
    v_res->size = max_size;
    return v_res;
}

void vector_int_isub(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));

    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *end = p0 + min_size;

    while (p0 < end) {
        *p0++ -= *p1++;
    }
}

VectorInt_t* vector_int_sub(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));
    size_t max_size = m ^ ((m ^ n) & -(m < n));
    
    VectorInt_t *v_res = vector_int_init(max_size);
    if (!v_res) return NULL;
    
    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *pr = v_res->ptr;
    int32_t *end = pr + min_size;
    
    // Phần giao nhau
    while (pr < end) {
        *pr++ = *p0++ - *p1++;
    }

    size_t mask = -(size_t)(v0->size > v1->size);
    int32_t *src_remaining = (int32_t*)(((uintptr_t)(v0->ptr + min_size) & mask) | ((uintptr_t)(v1->ptr + min_size) & ~mask));
    size_t remaining_len = max_size - min_size;
    int32_t multiplier = 1 - ((v0->size < v1->size) << 1);
    end += remaining_len;
    while (pr < end) {
        *pr++ = *src_remaining++ * multiplier;
    }
    v_res->size = max_size;
    return v_res;
}

void vector_int_imul(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));

    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *end = p0 + min_size;
    
    while (p0 < end) {
        *p0++ *= *p1++;
    }
}

VectorInt_t* vector_int_mul(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));
    size_t max_size = m ^ ((m ^ n) & -(m < n));
    
    VectorInt_t *v_res = vector_int_init(max_size);
    if (!v_res) return NULL;
    
    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *pr = v_res->ptr;
    int32_t *end = pr + min_size;
    
    // Phần giao nhau
    while (pr < end) {
        *pr++ = *p0++ * *p1++;
    }

    v_res->size = max_size;
    return v_res;
}

int64_t vector_int_dotproduct(VectorInt_t *restrict v0, VectorInt_t *restrict v1) {
    int64_t total = 0;

    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));

    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *end = p0 + min_size;
    while (p0 < end) {
        total += *p0++ * *p1++;
    }

    return total;
}

int64_t vector_int_sum(VectorInt_t *restrict v) {
    int64_t total = 0;

    size_t size = v->size;

    int32_t *ptr = v->ptr;
    int32_t *end = ptr + size;

    while (ptr < end) {
        total += *ptr++;
    }

    return total;
}

void vector_int_axpy(VectorInt_t *restrict v0, VectorInt_t *restrict v1, int32_t a) {
    // v1 = a * v0 + v1
    size_t m = v0->size;
    size_t n = v1->size;
    size_t min_size = n ^ ((m ^ n) & -(m < n));
    int32_t *p0 = v0->ptr;
    int32_t *p1 = v1->ptr;
    int32_t *end = p0 + min_size;

    while (p1 < end) {
        *p1++ += *p0++ *a;
    }
}