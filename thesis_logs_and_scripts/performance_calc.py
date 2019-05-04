#!/usr/bin/env python3.7

import matplotlib.pyplot as plt

service1_iterations = []
service1_time = []

app2_iterations = []
app2_time = []

with open("./keys_service1_short") as f:
	for line in f:
		service1_iterations.append(int(line.split()[0]))
		service1_time.append(int(line.split()[3]))
		#print(line.split()[3])


with open("./keys_app2_short") as f:
	for line in f:
		app2_iterations.append(int(line.split()[0]))
		app2_time.append(int(line.split()[3]))
		#print(line.split()[3])


service1_iterations_total = 0
service1_time_total = 0

app2_iterations_total = 0
app2_time_total = 0

for item in service1_iterations:
	service1_iterations_total = service1_iterations_total + item

for item in service1_time:
	service1_time_total = service1_time_total + item

for item in app2_iterations:
	app2_iterations_total = app2_iterations_total + item

for item in app2_time:
	app2_time_total = app2_time_total + item		


print("total iterations for service1 = ", service1_iterations_total)
print("total time for service1 = ", service1_time_total, " milliseconds ")
print("total iterations for app2 = ", app2_iterations_total)
print("total time for app2 = ", app2_time_total, " milliseconds \n")
#print(service1_iterations)

print("iterations per second for service1 = ", (service1_iterations_total / service1_time_total)*1000)
print("iterations per second for app2 = ", (app2_iterations_total / app2_time_total)*1000)

plt.plot(service1_iterations, service1_time, color="green", linewidth=2.5)
plt.xlabel('iterations')
plt.ylabel('time ms')
plt.title('Service1')
plt.show()


plt.plot(app2_iterations, app2_time, color="green", linewidth=2.5)
plt.xlabel('iterations')
plt.ylabel('time ms')
plt.title('App2')
plt.show()
