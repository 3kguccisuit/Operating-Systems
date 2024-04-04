import random

def FCFS(requests, head):
    head_movement = 0
    current_position = head

    for request in requests:
        head_movement += abs(current_position - request)
        current_position = request

    return head_movement

def SSTF(requests, head):
    head_movement = 0
    current_position = head
    remaining_requests = requests.copy()

    while remaining_requests:
        nearest_request = min(remaining_requests, key=lambda r: abs(r - current_position))
        head_movement += abs(current_position - nearest_request)
        current_position = nearest_request
        remaining_requests.remove(nearest_request)

    return head_movement

def SCAN(requests, head, min_cylinder=0):
    head_movement = 0
    current_position = head
    sorted_requests = sorted(requests)

    for request in reversed(sorted_requests):
        if request <= current_position:
            head_movement += current_position - request
            current_position = request

    if current_position > min_cylinder:
        head_movement += current_position - min_cylinder
        current_position = min_cylinder

    for request in sorted_requests:
        if request > head:
            head_movement += request - current_position
            current_position = request

    return head_movement

def C_SCAN(requests, head, max_cylinder, min_cylinder=0):
    head_movement = 0
    current_position = head
    sorted_requests = sorted(requests)

    for request in sorted_requests:
        if request >= current_position:
            head_movement += request - current_position
            current_position = request

    head_movement += max_cylinder - 1 - current_position
    current_position = min_cylinder
    head_movement += max_cylinder - 1 - min_cylinder


    for request in sorted_requests:
        if request <= head:
            head_movement += request - current_position
            current_position = request

    return head_movement

def LOOK(requests, head):
    head_movement = 0
    current_position = head
    sorted_requests = sorted(requests)

    right_requests = [request for request in sorted_requests if request >= head]
    left_requests = [request for request in sorted_requests if request < head]

    for request in right_requests:
        head_movement += abs(current_position - request)
        current_position = request

    for request in reversed(left_requests):
        head_movement += abs(current_position - request)
        current_position = request

    return head_movement

def C_LOOK(requests, head):
    head_movement = 0
    current_position = head
    sorted_requests = sorted(requests)

    right_requests = [request for request in sorted_requests if request >= head]
    left_requests = [request for request in sorted_requests if request < head]

    for request in right_requests:
        head_movement += abs(current_position - request)
        current_position = request

    if left_requests:
        head_movement += abs(current_position - left_requests[0])
        current_position = left_requests[0]

    for request in left_requests[1:]:
        head_movement += abs(current_position - request)
        current_position = request

    return head_movement


requests = [random.randint(0, 4999) for _ in range(1000)]
initial_head_position = 50
max_cylinder = 5000  

j=0
while(j<5000):
   print("Initial head position =", j)
   requests = [random.randint(0, 4999) for _ in range(1000)]
   fcfs_movement = FCFS(requests, j)
   sstf_movement = SSTF(requests, j)
   scan_movement = SCAN(requests, j)
   c_scan_movement = C_SCAN(requests, j, max_cylinder)
   look_movement = LOOK(requests, j)
   c_look_movement = C_LOOK(requests, j)
   print("Total head movement for FCFS:", fcfs_movement)
   print("Total head movement for SSTF:", sstf_movement)
   print("Total head movement for SCAN:", scan_movement)
   print("Total head movement for C-SCAN:", c_scan_movement)
   print("Total head movement for LOOK:", look_movement)
   print("Total head movement for C-LOOK:", c_look_movement)
   j +=500
   



#Correct head movement
# FCFS: 510
# SSTF: 204
# SCAN: 226
# C-SCAN: 389
# LOOK: 291
# C-LOOK: 321

# requests= [176, 79, 34, 60, 92, 11, 41, 114]
# initial_head_position = 50
# max_cylinder = 200  





