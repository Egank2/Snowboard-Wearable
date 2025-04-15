//
//  SessionView.swift
//  SnoWin
//
//  Created by Aakash Mukherjee on 3/4/25.
//

import SwiftUI

struct SessionView: View {
    @State private var selectedDate = Date()
    @State private var selectedRun = "1st Run - Blue Diary"
    @State private var showingDatePicker = false
    @State private var showingRunPicker = false
    
    // Sample data
    let tricks: [(name: String, time: String, xp: Int, status: TrickStatus)] = [
        ("Jump (20m)", "10:30", 110, .perfect),
        ("Flip (3 sec)", "10:45", 89, .partial),
        ("360 Spin", "11:15", 75, .failed)
    ]
    
    var body: some View {
        ScrollView {
            VStack(spacing: 20) {
                // Profile Header
                profileHeader
                
                // Date and Run Selection
                dateRunSelectionView
                
                // Map View
                mapView
                
                // Basic Stats
                statsView
                
                // Tricks and Playback Row
                HStack(spacing: 16) {
                    playbackView
                        .frame(maxWidth: UIScreen.main.bounds.width * 0.3)
                    
                    tricksLogView
                        .frame(maxWidth: .infinity)
                }
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
    
    // MARK: - Subviews
    
    private var profileHeader: some View {
        HStack {
            Image(systemName: "person.circle.fill")
                .font(.system(size: 40))
                .foregroundColor(.blue)
            
            VStack(alignment: .leading) {
                Text("John Doe")
                    .font(.title3)
                    .fontWeight(.semibold)
                Text("Level 12 Snowboarder")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            Text("2,450 XP")
                .font(.headline)
                .padding(.horizontal, 12)
                .padding(.vertical, 6)
                .background(Color.blue.opacity(0.1))
                .cornerRadius(8)
        }
        .padding()
        .background(Color.white)
        .cornerRadius(16)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
    
    private var dateRunSelectionView: some View {
        VStack(spacing: 12) {
            Button(action: { showingDatePicker.toggle() }) {
                HStack {
                    Text(selectedDate.formatted(.dateTime.day().month().year()))
                        .font(.title2)
                        .fontWeight(.semibold)
                    Image(systemName: "chevron.down")
                }
                .foregroundColor(.primary)
            }
            .sheet(isPresented: $showingDatePicker) {
                DatePicker("Select Date", selection: $selectedDate, displayedComponents: .date)
                    .datePickerStyle(.graphical)
                    .presentationDetents([.height(400)])
            }
            
            Button(action: { showingRunPicker.toggle() }) {
                HStack {
                    Text(selectedRun)
                        .font(.headline)
                    Image(systemName: "chevron.down")
                }
                .padding(.horizontal, 16)
                .padding(.vertical, 8)
                .background(Color.blue.opacity(0.1))
                .cornerRadius(8)
            }
            .sheet(isPresented: $showingRunPicker) {
                List {
                    ForEach(["1st Run - Blue Diary", "2nd Run - Red Route", "3rd Run - Black Diamond"], id: \.self) { run in
                        Button(action: {
                            selectedRun = run
                            showingRunPicker = false
                        }) {
                            Text(run)
                        }
                    }
                }
                .presentationDetents([.height(200)])
            }
        }
    }
    
    private var mapView: some View {
        ZStack {
            RoundedRectangle(cornerRadius: 16)
                .fill(Color.white)
                .shadow(color: .black.opacity(0.05), radius: 5)
            
            // Example mountain slope visualization
            Path { path in
                path.move(to: CGPoint(x: 50, y: 150))
                path.addCurve(
                    to: CGPoint(x: 350, y: 150),
                    control1: CGPoint(x: 150, y: 50),
                    control2: CGPoint(x: 250, y: 100)
                )
            }
            .stroke(Color.blue, lineWidth: 3)
            
            // Example route path
            Path { path in
                path.move(to: CGPoint(x: 70, y: 140))
                path.addQuadCurve(
                    to: CGPoint(x: 330, y: 140),
                    control: CGPoint(x: 200, y: 80)
                )
            }
            .stroke(Color.red, style: StrokeStyle(lineWidth: 2, dash: [5]))
            
            // Example markers
            ForEach([80, 200, 320], id: \.self) { x in
                Circle()
                    .fill(Color.blue)
                    .frame(width: 8, height: 8)
                    .position(x: CGFloat(x), y: 135)
            }
        }
        .frame(height: 200)
    }
    
    private var statsView: some View {
        HStack {
            StatBox(title: "Duration", value: "01:50", unit: "hrs")
            StatBox(title: "Distance", value: "5.0", unit: "km")
            StatBox(title: "XP Earned", value: "500", unit: "pts")
            StatBox(title: "Avg Speed", value: "32", unit: "km/h")
        }
    }
    
    private var tricksLogView: some View {
        VStack(alignment: .leading, spacing: 12) {
            Text("Tricks")
                .font(.headline)
            
            ForEach(tricks, id: \.name) { trick in
                TrickRow(
                    name: trick.name,
                    time: trick.time,
                    xp: trick.xp,
                    status: trick.status
                )
            }
        }
        .padding()
        .background(Color.white)
        .cornerRadius(16)
        .shadow(color: .black.opacity(0.05), radius: 5)
    }
    
    private var playbackView: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text("3D Playback")
                .font(.headline)
            
            ZStack {
                RoundedRectangle(cornerRadius: 16)
                    .fill(Color.white)
                
                // Example 3D visualization
                VStack {
                    Image(systemName: "figure.snowboarding")
                        .font(.system(size: 40))
                        .foregroundColor(.blue)
                    
                    Rectangle()
                        .fill(
                            LinearGradient(
                                gradient: Gradient(colors: [.white, .blue.opacity(0.3)]),
                                startPoint: .top,
                                endPoint: .bottom
                            )
                        )
                        .frame(height: 2)
                        .padding(.horizontal)
                    
                    HStack(spacing: 16) {
                        Button(action: {}) {
                            Image(systemName: "play.fill")
                                .foregroundColor(.blue)
                        }
                        Button(action: {}) {
                            Image(systemName: "arrow.clockwise")
                                .foregroundColor(.blue)
                        }
                    }
                }
            }
            .frame(height: 150)
            .shadow(color: .black.opacity(0.05), radius: 5)
        }
    }
}
