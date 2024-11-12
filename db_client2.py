import pyarrow as pa
import pyarrow.flight
import time 



## 方式二：
## 首先直接连接感兴趣的flight客户端，获取全部的flight info
## 接着通过筛选感兴趣的flight info (通过内容ID)，获取ticket
## 然后通过ticket直接获取对应的record reader 
## 最后通过reader按步加载对应的recordbatch
## 约束：从语义数据服务中获取拿到传输的客户端地址


client = pa.flight.connect("grpc://0.0.0.0:6667");

step_set = []
ticket_set = []
n_flights = 0
for flight_info in client.list_flights():
    n_flights += 1
    step = flight_info.total_records
    step_set.append(step) 
    print("this databox has ", step, " recordbatch");
    print("fligt", n_flights, ":")
    start_time = time.time()
    ticket = flight_info.endpoints[0].ticket
    ticket_set.append(ticket)
    print(flight_info.descriptor)
    print(ticket)


for i in range(len(ticket_set)):
    reader = client.do_get(ticket_set[i])
    end_time = time.time()
    elapsed_time = end_time - start_time
    print("do get time:", elapsed_time, "秒")
    recordReader = reader.to_reader()

    for i in range(step_set[i]):
        record = recordReader.read_next_batch()
        table = record.to_pandas()
        print(table)

client.close()

# client = pa.flight.connect("grpc://0.0.0.0:6666");
# reader = client.do_get(pa.flight.Ticket(b"weather"))
# tables = reader.read_all()
# print(tables)

