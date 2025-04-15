import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def compute_yaw_from_quat(df):
    """
    Compute the yaw angle (in radians) from quaternion components.
    The conversion uses the formula:
       yaw = atan2(2*(w*z + x*y), 1 - 2*(y^2 + z^2))
    """
    df["gyro_yaw"] = np.arctan2(
        2 * (df["quat_w"] * df["quat_z"] + df["quat_x"] * df["quat_y"]),
        1 - 2 * (df["quat_y"]**2 + df["quat_z"]**2)
    )
    return df

def estimate_yaw_rate(df, fs=100):
    """
    Estimate the yaw rate by computing the numerical gradient
    of the computed yaw angle.
    """
    dt = 1.0 / fs
    df["yaw_rate"] = np.gradient(df["gyro_yaw"], dt)
    return df

def detect_air_events_quat(yaw_rate, gps_speed, fs, yaw_thresh_deg=200, speed_dip=0.8):
    """
    Detect possible trick events when there is a rapid change in yaw angle and a dip in GPS speed.
    """
    spin_mask = np.abs(np.rad2deg(yaw_rate)) > yaw_thresh_deg
    mean_speed = pd.Series(gps_speed).rolling(10, min_periods=1).mean()
    speed_mask = gps_speed < mean_speed * speed_dip
    candidates = np.where(spin_mask & speed_mask)[0]

    impacts, current = [], []
    for i in candidates:
        if current and i == current[-1] + 1:
            current.append(i)
        else:
            if current: 
                impacts.append(current[-1])
            current = [i]
    if current:
        impacts.append(current[-1])
    return impacts

def integrate_spin(yaw_rate, impact_idx, fs, lookback_s=2.0):
    """
    Integrate the yaw rate backward from the impact index over a specified lookback period
    to estimate the total change in yaw (spin) in degrees.
    """
    start = max(0, impact_idx - int(lookback_s * fs))
    total_rad = np.sum(yaw_rate[start:impact_idx]) * (1 / fs)
    return np.rad2deg(total_rad)

def label_spin(total_deg, tol=40):
    """
    Label the trick based on the total degrees spun.
    The label matches the closest standard spin value if within tolerance.
    """
    spins = np.array([0, 180, 360, 540, 720, 900, 1080, 1260])
    nearest = spins[np.argmin(np.abs(spins - np.abs(total_deg)))]
    return "unknown" if abs(total_deg - nearest) > tol else f"{nearest}-spin"

def analyze_tricks(df, fs=200):
    """
    Process the DataFrame by computing yaw rate, detecting events and integrating the spin.
    Returns a DataFrame with indices, computed spin degrees, and labels.
    """
    df = estimate_yaw_rate(df, fs)
    impacts = detect_air_events_quat(df["yaw_rate"].values, df["gps_speed"].values, fs)

    trick_events = []
    for idx in impacts:
        spin_deg = integrate_spin(df["yaw_rate"].values, idx, fs)
        label = label_spin(spin_deg)
        trick_events.append({
            "impact_index": idx,
            "spin_deg": spin_deg,
            "spin_label": label
        })

    return pd.DataFrame(trick_events)

def plot_results(df, trick_df):
    """
    Plot the yaw and yaw rate along with detected trick events for visual inspection.
    """
    plt.figure(figsize=(12, 6))
    plt.plot(df["gyro_yaw"], label="Yaw (rad)")
    plt.plot(df["yaw_rate"], label="Yaw Rate (rad/s)")
    for _, ev in trick_df.iterrows():
        plt.axvline(ev["impact_index"], color='red', linestyle='--', label="Trick Impact")
    plt.legend()
    plt.title("Yaw and Detected Trick Landings")
    plt.xlabel("Sample Index")
    plt.ylabel("Angle / Angular Velocity")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

def main(df, fs=200):
    """
    Main function to process the input DataFrame.
    It calculates the yaw angle, detects trick events, and plots the results.
    """
    # Compute yaw from quaternion values
    df = compute_yaw_from_quat(df)
    
    # Analyze the detected trick events
    trick_df = analyze_tricks(df, fs)

    if trick_df.empty:
        print("No tricks were detected in this run.")
    else:
        print("Detected Tricks:")
        print(trick_df)
        plot_results(df, trick_df)

if __name__ == "__main__":
    df = pd.read_csv(
    "runs/run9.csv",
    header=None,
    names=["timestamp", "quat_w", "quat_x", "quat_y", "quat_z", "lat", "lon", "gps_speed", "altitude"]
)
    
    # Adjust the sampling frequency if needed; default fs is 100 Hz.
    main(df, fs=200)