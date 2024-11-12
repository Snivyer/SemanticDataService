import pyarrow as pa
import pyarrow.flight
import time 



## 方式三：
## 首先根据内容ID构建需要访问的Flight Descriptor
## 接着通过对应的flight客户端，然后提交Descriptor获取对应的flight info
## 然后从flight info中拿去ticket, 通过ticket直接获取对应的record reader 
## 最后通过reader按步加载对应的recordbatch
## 约束：从语义数据服务中获取拿到传输的客户端地址,并且需要知道的内容ID

cntID =["1", "2", "3"]
desc = pa.flight.FlightDescriptor.for_path(*cntID)
print("desc:", desc.path)

client = pa.flight.connect("grpc://0.0.0.0:6667")
flight_info = client.get_flight_info(desc)
step = flight_info.total_records;

print("this databox has ", step, " recordbatch");
start_time = time.time()
reader = client.do_get(flight_info.endpoints[0].ticket)
end_time = time.time()
elapsed_time = end_time - start_time
print("do get time:", elapsed_time, "秒")
recordReader = reader.to_reader()
for i in range(step):
    record = recordReader.read_next_batch()
    table = record.to_pandas()
    print(table)

client.close()

