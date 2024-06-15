from flask import Flask, request, jsonify

app = Flask(__name__)

# Define the base variable globally
base = []

@app.route('/data', methods=['POST'])
def receive_data():
    global base
    data = request.get_json()
    temp_data = data['temperature']
    hum_data = data['humidity']
    intensitas = data['Lux']
    pengunjung = data['pengunjung']
    SubData = [temp_data, hum_data, intensitas, pengunjung]

    # Update the global base variable with received data
    base = SubData

    # Log the received data
    print("Data diterima")
    print("- Suhu:", temp_data, "°C, Kelembaban:", hum_data, "%")
    print("- Intensitas Cahaya:", intensitas,", lux :", intensitas, ", Pengunjung:", pengunjung)
    
    # Send response
    response = {'message': 'Data diterima'}
    return jsonify(response), 200

@app.route('/data', methods=['GET'])
def dapat_data():
    global base
    # Return the base data as a JSON response
    return jsonify({'Temperature': base[0], 'Humidity': base[1], 'Intensitas Cahaya': base[2], 'Pengunjung' : base [3]}) if base else jsonify({'message': 'No data available'})

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
