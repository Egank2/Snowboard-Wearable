//
//  LeaderboardView.swift
//  SnoWin
//
//  Created by Aakash Mukherjee on 3/4/25.
//

import SwiftUI

struct LeaderboardView: View {
    @State private var selectedPeriod = "Weekly"
    let periods = ["Daily", "Weekly", "Monthly", "All Time"]
    
    var body: some View {
        ScrollView {
            VStack(spacing: 24) {
                // Period Selector
                Picker("Time Period", selection: $selectedPeriod) {
                    ForEach(periods, id: \.self) { period in
                        Text(period).tag(period)
                    }
                }
                .pickerStyle(.segmented)
                .padding(.horizontal)
                
                // Top Performers
                topPerformersSection
                
                // Categories
                categoriesSection
                
                // Friends Ranking
                friendsRankingSection
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
    
    private var topPerformersSection: some View {
        VStack(spacing: 16) {
            Text("Top Performers")
                .font(.headline)
            
            HStack(spacing: 0) {
                // Second Place
                LeaderboardPodium(rank: 2, name: "Alex Kim", score: "2,850", podiumColor: .gray)
                    .offset(y: 20)
                
                // First Place
                LeaderboardPodium(rank: 1, name: "Sarah Chen", score: "3,200", podiumColor: .yellow)
                    .zIndex(1)
                
                // Third Place
                LeaderboardPodium(rank: 3, name: "Mike Ross", score: "2,600", podiumColor: .brown)
                    .offset(y: 40)
            }
        }
    }
    
    private var categoriesSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Categories")
                .font(.headline)
            
            VStack(spacing: 12) {
                CategoryRow(title: "Highest Jump", leader: "Sarah Chen", value: "3.2m")
                CategoryRow(title: "Top Speed", leader: "Mike Ross", value: "85 km/h")
                CategoryRow(title: "Most Tricks", leader: "Alex Kim", value: "42")
            }
            .padding()
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
    
    private var friendsRankingSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Friends Ranking")
                .font(.headline)
            
            VStack(spacing: 12) {
                ForEach(1...5, id: \.self) { rank in
                    RankingRow(
                        rank: rank,
                        name: ["You", "Chris Wong", "Emma Davis", "Tom Wilson", "Lisa Park"][rank - 1],
                        score: [2450, 2200, 2100, 1950, 1800][rank - 1]
                    )
                }
            }
            .padding()
            .background(Color.white)
            .cornerRadius(16)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
}
