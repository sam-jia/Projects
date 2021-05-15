from flask import Blueprint, Flask, redirect, url_for, render_template, request, session, flash
from . import db
from flask_login import login_required, current_user, logout_user

main = Blueprint('main', __name__)

@main.route("/home")
@main.route("/")
def home():
    return render_template("home.html")

@main.route("/base")
def base():
    return render_template("base.html")
@main.route("/about-us")
@main.route("/About-us")
@main.route("/about_us")
@main.route("/About_us")
def about():
    return render_template("About_us.html")

@main.route("/gallery")
@main.route("/Gallery")
def gallery():
    return render_template("Gallery.html")

@main.route("/FAQ")
def FAQ():
    return render_template("base.html")

@main.route("/Keyboards")
@main.route("/keyboards")
def keyboards():
    return render_template("base.html")

@main.route("/Keycaps")
@main.route("/keycaps")
def keycaps():
    return render_template("base.html")

@main.route("/Switches")
@main.route("/switches")
def switches():
    return render_template("base.html")

@main.route("/Group-buys")
@main.route("/group-buys")
@main.route("/Group_buys")
@main.route("/group_buys")
def group_buys():
    return render_template("base.html")

@main.route("/Others")
@main.route("/others")
def others():
    return render_template("base.html")

@main.route("/Profile")
@main.route("/profile")
def profile():
    return render_template("profile.html")
