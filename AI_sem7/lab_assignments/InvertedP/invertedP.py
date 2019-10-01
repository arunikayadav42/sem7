import time

def profile(x, epsilon1, epsilon2, epsilon3):
    y = -1
    epsilon4 = epsilon3 + epsilon2 - epsilon1
    # print(epsilon1, epsilon2, epsilon3, epsilon4, x)
    if epsilon1 <= x <= epsilon2:
        y = 1.0 * (x - epsilon1) / (epsilon2 - epsilon1)

    elif epsilon2 < x <= epsilon3:
        y = 1.0

    elif epsilon3 < x <= epsilon4:
        y = 1.0 *(x - epsilon4) / (epsilon3 - epsilon4)
    return y


def fuzzify(x, epsilon):
    epsilon1 = -1 * epsilon[0]
    epsilon2 = 0
    epsilon3 = 0
    y_zero = profile(x, epsilon1, epsilon2, epsilon3)
   
    epsilon1 = 0
    epsilon2 = epsilon[1]
    epsilon3 = epsilon[2]
    y_small_positive = profile(x, epsilon1, epsilon2, epsilon3)

    epsilon1 = -1 * (epsilon[1] + epsilon[2])
    epsilon2 = -1 * (epsilon[2])
    epsilon3 = -1 * (epsilon[1])
    y_small_negative = profile(x, epsilon1, epsilon2, epsilon3)

    return y_small_negative, y_zero, y_small_positive


def rules(theta, omega, epsilon_theta, epsilon_omega):
    y_theta = fuzzify(theta, epsilon_theta)
    y_omega = fuzzify(omega, epsilon_omega)
    print("y_theta : ", y_theta)
    print("y_omega : ", y_omega)

    control_matrix = {'00': 2,
                      '01': 1,
                      '02': 0,
                      '10': 1,
                      '11': 0,
                      '12': -1,
                      '20': 0,
                      '21': -1,
                      '22': -2}

    y_current = []
    for theta_index, theta_value in enumerate(y_theta):
        for omega_index, omega_value in enumerate(y_omega):
            if(theta_value == -1 or omega_value == -1):
                continue
            else :
                curr_belongingness = min(theta_value, omega_value)
                curr_index = control_matrix[str(theta_index) + str(omega_index)]
                y_current.append([curr_belongingness, curr_index])     

    return y_current

def defuzzify(epsilon, height):
    epsilon4 = epsilon[2] + epsilon[1] - epsilon[0]
    base1 = epsilon4 - epsilon[0]
    base2 = base1 - 2*(epsilon[1] - epsilon[0])
    area = 0.5 * (base1+ base2) * height   
    x_centroid = (epsilon[0] + epsilon4) / 2 
    return x_centroid, area

def calculate_current(theta, omega, epsilon_theta, epsilon_omega, epsilon_curr):
    y_current = rules(theta, omega, epsilon_theta, epsilon_omega)
    print(y_current)
    current_profile = {
        -2 : [-1*(epsilon_curr[5] + epsilon_curr[4] - epsilon_curr[3]), -1 * epsilon_curr[5], -1 * epsilon_curr[4]],
        -1 : [-1 * (epsilon_curr[1] + epsilon_curr[2]), -1 * (epsilon_curr[2]), -1 * epsilon_curr[1]],
        0 : [-1 * epsilon_curr[0], 0, 0],
        1 : [0 , epsilon_curr[1], epsilon_curr[2]], 
        2 : [epsilon_curr[3], epsilon_curr[4], epsilon_curr[5]]
    }

    weighted_avg = 0
    total_area = 0
    for current in y_current:
        centroid, area = defuzzify(current_profile[current[1]], current[0])
        weighted_avg += area * centroid
        total_area += area

    return weighted_avg / total_area

def main():
    epsilon_theta = [3, 2, 5]
    epsilon_omega = [2, 4 ,6]
    epsilon_curr = [2, 3, 5, 4, 6, 8]
    theta = 1
    omega = 1
    current = calculate_current(theta, omega, epsilon_theta, epsilon_omega, epsilon_curr)
    print(current)
    while True:
        time.sleep(10)
        theta = theta + omega/10 + current/200
        omega = omega + current/10
        print(theta, omega)
        current = calculate_current(theta, omega, epsilon_theta, epsilon_omega, epsilon_curr)
        print(current)


if __name__ == "__main__":
    main()