import time
import random

# --- 1. Đo chi phí Import ---
t0 = time.perf_counter()
import numpy as np
t1 = time.perf_counter()
import_np_time = t1 - t0

t0 = time.perf_counter()
from vector import VectorInt
t1 = time.perf_counter()
import_vec_time = t1 - t0

# Thiết lập dữ liệu (không tính vào thời gian import)
SIZE = 1_000_000
data = [random.randint(1, SIZE) for _ in range(SIZE)]

# --- 2. Đo chi phí Load dữ liệu ---
# Đo cho NumPy
t0 = time.perf_counter()
arr = np.array(data, dtype=np.int32)
load_np_time = time.perf_counter() - t0

# Đo cho VectorInt
t0 = time.perf_counter()
v = VectorInt(data)
load_vec_time = time.perf_counter() - t0

# --- 3. Đo chi phí Thực thi (Sum) ---
# NumPy
t0 = time.perf_counter()
_ = np.sum(arr)
sum_np_time = time.perf_counter() - t0

# VectorInt
t0 = time.perf_counter()
_ = v.sum()
sum_vec_time = time.perf_counter() - t0

# --- 4. Đo chi phí Thực thi (Sort) ---
t0 = time.perf_counter()
_ = arr.sort()
sort_np_time = time.perf_counter() - t0

t0 = time.perf_counter()
_ = v.sort()
sort_vec_time = time.perf_counter() - t0

# --- 5. Đo chi phí Thực thi (Search) ---
t0 = time.perf_counter()
_ = np.where(arr == 8)
find_np_time = time.perf_counter() - t0

t0 = time.perf_counter()
_ = v.findall(8)
find_vec_time = time.perf_counter() - t0

# --- 6. Đo chi phí THực thi (Dot Product) ---
a2 = np.array(data, dtype=np.int32)
t0 = time.perf_counter()
_ = np.dot(arr, a2)
dot_np_time = time.perf_counter() - t0

v2 = VectorInt(data)
t0 = time.perf_counter()
_ = v.dot_product(v2)
dot_vec_time = time.perf_counter() - t0

total_np_time = import_np_time + load_np_time + sum_np_time + sort_np_time + find_np_time + dot_np_time
total_vec_time = import_vec_time + load_vec_time + sum_vec_time + sort_vec_time + find_vec_time + dot_vec_time
# --- Kết quả ---
print(f"{'Giai đoạn':<20} | {'NumPy (s)':<12} | {'VectorInt (s)':<12}")
print("-" * 50)
print(f"{'Import':<20} | {import_np_time:<12.6f} | {import_vec_time:<12.6f}")
print(f"{'Load dữ liệu':<20} | {load_np_time:<12.6f} | {load_vec_time:<12.6f}")
print(f"{'Thực thi (sum)':<20} | {sum_np_time:<12.6f} | {sum_vec_time:<12.6f}")
print(f"{'Thực thi (sort)':<20} | {sort_np_time:<12.6f} | {sort_vec_time:<12.6f}")
print(f"{'Thực thi (find)':<20} | {find_np_time:<12.6f} | {find_vec_time:<12.6f}")
print(f"{'Thực thi (dot)':<20} | {dot_np_time:<12.6f} | {dot_vec_time:<12.6f}")
print(f"{'TỔNG CỘNG':<20} | {(total_np_time):<12.6f} | {(total_vec_time):<12.6f}")