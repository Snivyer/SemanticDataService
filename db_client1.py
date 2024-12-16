import pyarrow as pa
import pyarrow.flight
import time 



## 方式一：
## 首先通过查询flight元数据服务器，获取全部的flight info信息；
## 接着通过筛选感兴趣的flight info (通过内容ID)，获取recorder reader
## 通过reader按步加载对应的recordbatch
## 约束：只需要记住最近的服务器IP和端口即可

metadata_client = pa.flight.connect("grpc://0.0.0.0:4444");

n_flights = 0
for flight_info in metadata_client.list_flights():
    n_flights += 1
    step = flight_info.total_records;
    print("fligt", n_flights, ":");
    print("this databox has ", step, " recordbatch");

    client =  pa.flight.connect(flight_info.endpoints[0].locations[0].uri);
    start_time = time.time()
    ticket = flight_info.endpoints[0].ticket
    print(flight_info.descriptor)

    reader = client.do_get(ticket)
    end_time = time.time()
    elapsed_time = end_time - start_time
    print("do get time:", elapsed_time, "秒")
    recordReader = reader.to_reader()

    for i in range(step):
        record = recordReader.read_next_batch()
        table = record.to_pandas()
        print(table)
    client.close()
   

# client = pa.flight.connect("grpc://0.0.0.0:6666");
# reader = client.do_get(pa.flight.Ticket(b"weather"))
# tables = reader.read_all()
# print(tables)

