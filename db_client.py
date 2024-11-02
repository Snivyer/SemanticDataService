import pyarrow as pa
import pyarrow.flight
import time 


client = pa.flight.connect("grpc://0.0.0.0:4444")

start_time = time.time()
reader = client.do_get(pa.flight.Ticket(b"weather"))
table = reader.read_pandas()
end_time = time.time()
elapsed_time = end_time - start_time
print(table)
print("do get time:", elapsed_time, "秒")

