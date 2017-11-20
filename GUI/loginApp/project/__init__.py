# project/__init__.py


from flask import Flask, request, jsonify, session
from models import User
from passlib.hash import pbkdf2_sha256

# config

app = Flask(__name__)
app.config['SECRET_KEY'] = "hello"
filename = 'firebase.txt'

# routes

@app.route('/')
def index():
    return app.send_static_file('index.html')


@app.route('/api/register', methods=['POST'])
def register():
    json_data = request.json
    user = User(email=json_data['email'], password=hashPassword(json_data['password']))
    if checkIfRegistered(user):
        status = 'This user is already registered'
    else:
        registerUser(user)
        status = 'success'
    return jsonify({'result': status})

@app.route('/api/login', methods=['POST'])
def login():
    json_data = request.json
    user = User(email=json_data['email'], password=json_data['password'])
    if checkIfRegistered(user) and validate(user):
        session['logged_in'] = True
        status = True
    else:
        status = False;
    user.password = hashPassword(user.password)
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

def hashPassword(password):
    return pbkdf2_sha256.encrypt(password, rounds=200000, salt_size=16)

def verifyPassword(password, challenge):
    return pbkdf2_sha256.verify(password, challenge)
    
def checkIfRegistered(user):
    try:
        f = open(filename, 'r+')
    except:
        f = open(filename, 'w+')
        f.close()
    f = open(filename, 'r+')
    lines = f.readlines()
    for l in lines:
        if user.email in l:
            f.close()
            return True
    f.close()
    return False

def registerUser(user):
    f = open(filename, 'a+')
    f.write('{0}'.format(user))
    f.close()

def validate(user):
    f = open(filename, 'r+')
    lines = f.readlines()
    for l in lines:
        if user.email in l:
            arr = l.split(' ')
            return verifyPassword(user.password, arr[2])
    return False
    

if __name__ == '__main__':
    app.debug = True
    app.run()
