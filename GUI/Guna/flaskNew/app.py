from flask import Flask, jsonify, send_from_directory

app = Flask(__name__)

@app.route('/api/v1/get_names')
def get_names():
    names = {'names':
             [
                 'fred',
                 'john',
                 'paul',
                 'greg'
             ]}
    return jsonify(names)

@app.route('/api/v1/get_people')
def get_people():
    people = {"people": [
        {'name': 'fred', 'age': 10},
        {'name': 'john', 'age': 20},
        {'name': 'paul', 'age': 30},
        {'name': 'greg', 'age': 40},
    ]}
    return jsonify(people)
@app.route('/api/show')
def triggerShow():
    global show 
    show = not show
    return jsonify({'vis': show})

@app.route('/api/login/<user>')
def get_time(user):
    return jsonify({'username': user})
@app.route('/api/<user>/get_devices')
def get_devices(user):
    return jsonify({ 'devices': ['dev1','dev2','dev3'] })


@app.route('/')
def index():
    return send_from_directory("templates", "index.html")



if __name__ == '__main__':
    app.debug = True
    show = True
    app.run(port=1234)