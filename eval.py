import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
 
connections = 8000
rates = range(50, 1001, 50)

# Define lists to store the results
response_times = []
reply_rates = []

try:
    # Iterate over the rates
    for rate in rates:
        # Run the httperf command and capture the output
        output = subprocess.check_output(
            [
                "httperf",
                "--server",
                "192.168.162.19",  # Server IP Address
                "--port",  # Server Port number
                "8095",
                "--num-conns",
                str(connections),
                "--rate",
                str(rate),
                "--timeout",
                "1",
                "/Trial.html",
            ]
        )

        # Decode the output bytes to a string
        output_str = output.decode()

        reply_rate = re.search(
            r"Reply rate \[replies/s\]: min (\d+\.\d+) avg (\d+\.\d+) max (\d+\.\d+)",
            output_str,
        )
        reply_time = re.search(
            r"Reply time \[ms\]: response (\d+\.\d+) transfer (\d+\.\d+)", output_str
        )
        print(
            "Rate: ",
            rate,
            "Reply Time: ",
            reply_time.group(1),
            "Reply Rate: ",
            reply_rate.group(2),
        )
        response_times.append(float(reply_time.group(1)))
        reply_rates.append(float(reply_rate.group(2)))
except:
    print("An Error or Exception Occured. Excel and Graphs are being generated.")
    pass

results_df = pd.DataFrame(
    {"Rate": rates, "Response Time": response_times, "Reply Rate": reply_rates}
)
results_df.to_excel("httperf_" + str(connections) + "_results.xlsx", index=False)

plt.plot(rates, response_times)
plt.xlabel("Rate")
plt.ylabel("Response Time")
plt.title("Latency Graph")
plt.savefig("latency_" + str(connections) + ".png")

plt.figure()  # Create a new figure
plt.plot(rates, reply_rates)
plt.xlabel("Rate")
plt.ylabel("Avg. Reply Rate")
plt.title("Throughput Graph")
plt.savefig("throughput_" + str(connections) + ".png")
