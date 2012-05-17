---
layout: default
title: Steinwurf
---

{% for post in site.posts limit:3 %}
<h2><a href="{{ post.url }}">{{ post.title }}</a></h2>
{{ post.content }}
<em>Posted on {{ post.date | date_to_long_string }}.</em>
{% endfor %}


