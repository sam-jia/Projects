from flask import Blueprint, render_template, redirect, url_for, request, flash
from . import db, bcrypt
from .models import User
from flask_login import login_user, logout_user, login_required, UserMixin, current_user
from .models import User
from .forms import RegistrationForm, LoginForm, RegistrationForm, ResetPasswordForm
from werkzeug.security import generate_password_hash, check_password_hash
from .utils import send_reset_email

auth = Blueprint('auth', __name__)

@auth.route('/login')
def login():
    return render_template('Login.html')

@auth.route('/login', methods=['POST', 'GET'])
def login_post():
    if current_user.is_authenticated:
        return redirect(url_for('main.home'))
    form = LoginForm()
    if form.validate_on_submit():
        user = User.query.filter_by(email=form.email.data).first()
        if user and bcrypt.check_password_hash(user.password, form.password.data):
            login_user(user, remember=form.remember.data)
            return redirect(url_for('main.home'))
        else:
            flash('Invalid credentials, please check your email and password')
            return redirect(url_for('auth.login'))

@auth.route('/signup')
def signup():
    return render_template('Sign_up.html')

@auth.route('/signup', methods=['POST', 'GET'])
def signup_post():
    if current_user.is_authenticated:
        return redirect(url_for('main.home'))

    form = RegistrationForm()
    if form.validate_on_submit():
        hashed_password = bcrypt.generate_password_hash(form.password.data).decode('utf-8')
        user = User(email=form.email.data, first_name=form.first_name.data, last_name=form.last_name.data, password=hashed_password)
        db.session.add(user)
        db.session.commit()
        flash('You account has been registered!')
        return redirect(url_for('auth.login'))

@auth.route('/logout')
@login_required
def logout():
    logout_user()
    return render_template('logout.html')

@auth.route('/reset_password', methods=['POST', 'GET'])
def reset_request():
    if current_user.is_authenticated:
        return redirect(url_for('main.home'))
    form = RequestResetForm()
    if form.validate_on_submit():
        user = User.query.filter_by(email=form.email.data).first()
        send_reset_email(user)
        flash("An email has been sent with instructions to reset your password.")
        return redirect(url_for('users.login'))
    return render_template('reset_request.html')

@auth.route('/reset_password/<token>', methods=['POST', 'GET'])
def reset_token(token):
    if current_user.is_authenticated:
        return redirect(url_for('main.home'))

    user = User.verify_reset_token(token)

    if user is None:
        flash('This is an invalid or expired link')
        return redirect(url_for('auth.reset_request'))
    form = ResetPasswordForm()
    if form.validate_on_submit():
        hashed_password = bcrypt.generate_password_hash(form.password.data).decode('utf-8')
        user.password = hashed_password
        dbsession.commit()
        flash('Your password has been updated!')
        return redirect(url_for('auth.login'))
    return render_template('reset_token.html')
