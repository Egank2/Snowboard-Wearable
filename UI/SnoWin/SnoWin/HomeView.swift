//
//  HomeView.swift
//  SnoWin
//
//  Created by Aakash Mukherjee on 3/4/25.
//

import SwiftUI

struct HomeView: View {
    @State private var selectedResort = "Whistler Blackcomb"
    
    var body: some View {
        ScrollView {
            VStack(spacing: 24) {
                // Weather and Snow Conditions
                weatherSection
                
                // Resort Selection
                resortSection
                
                // Equipment Check
                equipmentSection
                
                // News Feed
                newsFeedSection
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
    
    private var weatherSection: some View {
        VStack(spacing: 16) {
            HStack {
                Text("Weather & Conditions")
                    .font(.headline)
                Spacer()
                Text("Whistler, BC")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            HStack(spacing: 20) {
                VStack(spacing: 8) {
                    Image(systemName: "sun.max.fill")
                        .font(.system(size: 40))
                        .foregroundColor(.yellow)
                    Text("-2°C")
                        .font(.title2)
                        .fontWeight(.semibold)
                    Text("Sunny")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                
                Divider()
                
                VStack(alignment: .leading, spacing: 8) {
                    ConditionRow(title: "Snow Depth", value: "180cm", icon: "snow")
                    ConditionRow(title: "Fresh Snow", value: "15cm", icon: "cloud.snow")
                    ConditionRow(title: "Visibility", value: "Good", icon: "eye")
                }
            }
            .padding()
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
    
    private var resortSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Popular Resorts")
                .font(.headline)
            
            ScrollView(.horizontal, showsIndicators: false) {
                HStack(spacing: 16) {
                    ForEach(["Whistler Blackcomb", "Vail", "Park City", "Zermatt"], id: \.self) { resort in
                        ResortCard(name: resort, isSelected: resort == selectedResort)
                            .onTapGesture {
                                selectedResort = resort
                            }
                    }
                }
            }
        }
    }
    
    private var equipmentSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Equipment Status")
                .font(.headline)
            
            VStack(spacing: 12) {
                EquipmentRow(name: "SnoWin Device", status: "Connected", batteryLevel: 75)
                EquipmentRow(name: "Snowboard Bindings", status: "Check Required", batteryLevel: nil)
                EquipmentRow(name: "Smart Goggles", status: "Low Battery", batteryLevel: 15)
            }
            .padding()
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
    
    private var newsFeedSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Latest News")
                .font(.headline)
            
            VStack(spacing: 16) {
                NewsCard(
                    title: "New Snow Park Opening",
                    description: "Whistler Blackcomb announces new terrain park features",
                    date: "2h ago",
                    imageSystemName: "snowflake"
                )
                
                NewsCard(
                    title: "Weekend Weather Alert",
                    description: "Heavy snowfall expected this weekend",
                    date: "4h ago",
                    imageSystemName: "cloud.snow"
                )
            }
        }
    }
}
