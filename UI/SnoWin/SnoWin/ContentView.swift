//
//  ContentView.swift
//  SnoWin
//
//  Created by Aakash Mukherjee on 31/3/25.
//

import SwiftUI

struct ContentView: View {
    @State private var selectedTab = 0
    
    var body: some View {
        TabView(selection: $selectedTab) {
            HomeView()
                .tabItem {
                    Label("Home", systemImage: "house.fill")
                }
                .tag(0)
            
            SessionView()
                .tabItem {
                    Label("Session", systemImage: "figure.snowboarding")
                }
                .tag(1)
            
            ProfileView()
                .tabItem {
                    Label("Profile", systemImage: "person.fill")
                }
                .tag(2)
        }
        .tint(.blue)
    }
}



// MARK: - Supporting Views

struct StatBox: View {
    let title: String
    let value: String
    let unit: String
    
    var body: some View {
        VStack(spacing: 4) {
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
            
            Text(value)
                .font(.title3)
                .fontWeight(.semibold)
            
            Text(unit)
                .font(.caption2)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
        .padding(.vertical, 12)
        .background(Color.white)
        .cornerRadius(12)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
}

struct StatCard: View {
    let title: String
    let value: String
    let icon: String
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Image(systemName: icon)
                .font(.title2)
                .foregroundColor(.blue)
            
            VStack(alignment: .leading, spacing: 4) {
                Text(value)
                    .font(.title3)
                    .fontWeight(.semibold)
                Text(title)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color.white)
        .cornerRadius(12)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
}

struct TrickRow: View {
    let name: String
    let time: String
    let xp: Int
    let status: TrickStatus
    
    var body: some View {
        HStack {
            VStack(alignment: .leading) {
                Text(name)
                    .font(.subheadline)
                Text(time)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            Text("+\(xp) XP")
                .fontWeight(.medium)
                .foregroundColor(status.color)
        }
    }
}

enum TrickStatus {
    case perfect
    case partial
    case failed
    
    var color: Color {
        switch self {
        case .perfect: return .green
        case .partial: return .orange
        case .failed: return .red
        }
    }
}

struct ConditionRow: View {
    let title: String
    let value: String
    let icon: String
    
    var body: some View {
        HStack {
            Image(systemName: icon)
                .frame(width: 24)
            Text(title)
            Spacer()
            Text(value)
                .fontWeight(.medium)
        }
    }
}

struct ResortCard: View {
    let name: String
    let isSelected: Bool
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Image(systemName: "mountain.2.fill")
                .font(.system(size: 30))
                .foregroundColor(isSelected ? .blue : .gray)
            
            Text(name)
                .font(.subheadline)
                .fontWeight(.medium)
            
            Text("Open")
                .font(.caption)
                .foregroundColor(.green)
        }
        .frame(width: 120)
        .padding()
        .background(isSelected ? Color.blue.opacity(0.1) : Color.white)
        .cornerRadius(16)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
}

struct EquipmentRow: View {
    let name: String
    let status: String
    let batteryLevel: Int?
    
    var body: some View {
        HStack {
            VStack(alignment: .leading) {
                Text(name)
                    .font(.subheadline)
                Text(status)
                    .font(.caption)
                    .foregroundColor(status == "Connected" ? .green : .orange)
            }
            
            Spacer()
            
            if let battery = batteryLevel {
                BatteryIndicator(level: battery)
            }
        }
    }
}

struct BatteryIndicator: View {
    let level: Int
    
    var body: some View {
        HStack(spacing: 4) {
            Image(systemName: "battery.\(level)")
                .foregroundColor(level < 20 ? .red : .green)
            Text("\(level)%")
                .font(.caption)
                .foregroundColor(.secondary)
        }
    }
}

struct NewsCard: View {
    let title: String
    let description: String
    let date: String
    let imageSystemName: String
    
    var body: some View {
        HStack(spacing: 16) {
            Image(systemName: imageSystemName)
                .font(.system(size: 30))
                .foregroundColor(.blue)
                .frame(width: 60, height: 60)
                .background(Color.blue.opacity(0.1))
                .cornerRadius(12)
            
            VStack(alignment: .leading, spacing: 4) {
                Text(title)
                    .font(.subheadline)
                    .fontWeight(.medium)
                Text(description)
                    .font(.caption)
                    .foregroundColor(.secondary)
                Text(date)
                    .font(.caption2)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
        }
        .padding()
        .background(Color.white)
        .cornerRadius(16)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
}

struct LeaderboardPodium: View {
    let rank: Int
    let name: String
    let score: String
    let podiumColor: Color
    
    var body: some View {
        VStack(spacing: 8) {
            Image(systemName: "person.circle.fill")
                .font(.system(size: 50))
                .foregroundColor(.blue)
            
            Text(name)
                .font(.subheadline)
                .fontWeight(.medium)
            
            Text(score)
                .font(.caption)
                .foregroundColor(.secondary)
            
            Text("#\(rank)")
                .font(.title2)
                .fontWeight(.bold)
                .foregroundColor(.white)
                .frame(width: 100, height: rank == 1 ? 100 : rank == 2 ? 80 : 60)
                .background(podiumColor)
                .cornerRadius(16)
        }
        .frame(maxWidth: .infinity)
    }
}

struct CategoryRow: View {
    let title: String
    let leader: String
    let value: String
    
    var body: some View {
        HStack {
            Text(title)
                .font(.subheadline)
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text(leader)
                    .font(.subheadline)
                    .fontWeight(.medium)
                Text(value)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
    }
}

struct RankingRow: View {
    let rank: Int
    let name: String
    let score: Int
    
    var body: some View {
        HStack {
            Text("#\(rank)")
                .font(.subheadline)
                .fontWeight(.medium)
                .frame(width: 40)
            
            Image(systemName: "person.circle.fill")
                .font(.title3)
                .foregroundColor(.blue)
            
            Text(name)
                .font(.subheadline)
            
            Spacer()
            
            Text("\(score) XP")
                .font(.subheadline)
                .fontWeight(.medium)
        }
    }
}

// MARK: - Preview
#Preview {
    ContentView()
}
