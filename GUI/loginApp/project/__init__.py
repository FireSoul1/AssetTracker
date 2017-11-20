# project/__init__.py


from flask import Flask, request, jsonify, session
from models import User


# config

app = Flask(__name__)
app.config['SECRET_KEY'] = "hello"

# routes

@app.route('/')
def index():
    return app.send_static_file('index.html')


@app.route('/api/register', methods=['POST'])
def register():
    json_data = request.json
    user = User(
        email=json_data['email'],
        password=json_data['password']
    )
    # TODO Check to see if user is registered
    return jsonify({'result': True})


@app.route('/api/login', methods=['POST'])
def login():
    print("Logging in")
    json_data = request.json
    print(json_data)
    #user = User.query.filter_by(email=json_data['email']).first()
    session['logged_in'] = True
    status = True
    print("returning")
    return jsonify({'result': status})


@app.route('/api/logout')
def logout():
    session.pop('logged_in', None)
    return jsonify({'result': 'success'})


@app.route('/api/status')
def status():
    if session.get('logged_in'):
        if session['logged_in']:
            return jsonify({'status': True})
    else:
        return jsonify({'status': False})

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



if __name__ == '__main__':
    app.debug = True
    app.run()
