from flask_login import UserMixin
from itsdangerous import TimedJSONWebSignatureSerializer as Serializer
from . import db, create_app
from flask import current_app
class User(UserMixin, db.Model):
    #db.create_all()
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(100), unique=True)
    password = db.Column(db.String(100))
    first_name = db.Column(db.String(1000))
    last_name = db.Column(db.String(1000))
    
    def get_reset_token(self, expires_sec = 1800):
        s = Serializer(app.config['SECRET_KEY'], expires_sec)
        return s.dumps({'user_id': self.id}).decode('utf-8')
    
    def verify_reset_toke(token):
        s = Serializer(app.config['SECRET_KEY'])
        try: user_id = s.loads