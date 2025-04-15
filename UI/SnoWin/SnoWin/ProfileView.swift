//
//  ProfileView.swift
//  SnoWin
//
//  Created by Aakash Mukherjee on 3/4/25.
//

import SwiftUI

struct ProfileView: View {
    @State private var isDeviceConnected = false
    
    var body: some View {
        ScrollView {
            VStack(spacing: 24) {
                // Profile Header
                profileHeader
                
                // Device Connection
                deviceSection
                
                // Stats Overview
                statsOverview
                
                // Settings
                settingsSection
                
                // Social
                socialSection
            }
            .padding()
        }
        .background(
            LinearGradient(
                gradient: Gradient(colors: [
                    Color.blue.opacity(0.1),
                    Color.white
                ]),
                startPoint: .top,
                endPoint: .bottom
            )
        )
    }
    
    private var profileHeader: some View {
        VStack(spacing: 16) {
            Image(systemName: "person.circle.fill")
                .font(.system(size: 80))
                .foregroundColor(.blue)
            
            VStack(spacing: 4) {
                Text("John Doe")
                    .font(.title)
                    .fontWeight(.semibold)
                
                Text("Level 12 Snowboarder")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            HStack(spacing: 24) {
                VStack {
                    Text("2,450")
                        .font(.title3)
                        .fontWeight(.semibold)
                    Text("Total XP")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                
                VStack {
                    Text("15")
                        .font(.title3)
                        .fontWeight(.semibold)
                    Text("Sessions")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                
                VStack {
                    Text("8")
                        .font(.title3)
                        .fontWeight(.semibold)
                    Text("Badges")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
            }
        }
        .padding()
        .background(Color.white)
        .cornerRadius(16)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
    
    private var deviceSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Device")
                .font(.headline)
            
            VStack(spacing: 12) {
                HStack {
                    Image(systemName: "snowflake")
                        .font(.title2)
                        .foregroundColor(.blue)
                    
                    VStack(alignment: .leading) {
                        Text("SnoWin Device")
                            .font(.subheadline)
                            .fontWeight(.medium)
                        Text(isDeviceConnected ? "Connected" : "Not Connected")
                            .font(.caption)
                            .foregroundColor(isDeviceConnected ? .green : .secondary)
                    }
                    
                    Spacer()
                    
                    Button(action: { isDeviceConnected.toggle() }) {
                        Text(isDeviceConnected ? "Disconnect" : "Connect")
                            .font(.subheadline)
                            .fontWeight(.medium)
                            .foregroundColor(.white)
                            .padding(.horizontal, 16)
                            .padding(.vertical, 8)
                            .background(isDeviceConnected ? Color.red : Color.blue)
                            .cornerRadius(8)
                    }
                }
                
                if isDeviceConnected {
                    HStack {
                        Label("Battery", systemImage: "battery.75")
                        Spacer()
                        Text("75%")
                    }
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                    
                    HStack {
                        Label("Firmware", systemImage: "cpu")
                        Spacer()
                        Text("v1.2.3")
                    }
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                }
            }
            .padding()
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
    
    private var statsOverview: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Stats Overview")
                .font(.headline)
            
            LazyVGrid(columns: [
                GridItem(.flexible()),
                GridItem(.flexible())
            ], spacing: 16) {
                StatCard(title: "Highest Jump", value: "2.5m", icon: "arrow.up")
                StatCard(title: "Top Speed", value: "45 km/h", icon: "speedometer")
                StatCard(title: "Longest Run", value: "5.2 km", icon: "flag.filled")
                StatCard(title: "Perfect Tricks", value: "24", icon: "star.fill")
            }
        }
    }
    
    private var settingsSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Settings")
                .font(.headline)
            
            VStack(spacing: 0) {
                ForEach(["Profile Settings", "Notifications", "Privacy", "Help & Support"], id: \.self) { setting in
                    Button(action: {}) {
                        HStack {
                            Text(setting)
                            Spacer()
                            Image(systemName: "chevron.right")
                                .foregroundColor(.secondary)
                        }
                        .padding()
                    }
                    .foregroundColor(.primary)
                    
                    if setting != "Help & Support" {
                        Divider()
                    }
                }
            }
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
    
    private var socialSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Social")
                .font(.headline)
            
            VStack(spacing: 12) {
                // Instagram Connection
                HStack {
                    Image("instagram_logo")
                        .resizable()
                        .frame(width: 24, height: 24)
                    
                    Text("Instagram")
                        .font(.subheadline)
                    
                    Spacer()
                    
                    Button(action: {}) {
                        Text("Connect")
                            .font(.subheadline)
                            .foregroundColor(.white)
                            .padding(.horizontal, 16)
                            .padding(.vertical, 8)
                            .background(Color.blue)
                            .cornerRadius(8)
                    }
                }
                
                // Friends
                VStack(alignment: .leading, spacing: 8) {
                    Text("Friends")
                        .font(.subheadline)
                        .fontWeight(.medium)
                    
                    ScrollView(.horizontal, showsIndicators: false) {
                        HStack(spacing: 16) {
                            ForEach(["Chris", "Emma", "Tom", "Lisa", "Alex"], id: \.self) { friend in
                                VStack {
                                    Image(systemName: "person.circle.fill")
                                        .font(.system(size: 40))
                                        .foregroundColor(.blue)
                                    Text(friend)
                                        .font(.caption)
                                }
                            }
                        }
                    }
                }
            }
            .padding()
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
}
