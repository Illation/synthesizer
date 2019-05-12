// -*- c++ -*-
// Generated by gmmproc 2.47.3.1 -- DO NOT MODIFY!
#ifndef _ATKMM_OBJECT_H
#define _ATKMM_OBJECT_H


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/* $Id: object.hg,v 1.5 2004/11/30 21:41:44 murrayc Exp $ */

/* Copyright (C) 1998-2002 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <glibmm/object.h>
#include <atkmm/component.h>
#include <atkmm/relation.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern "C" { typedef struct _AtkPropertyValues AtkPropertyValues; }
#endif


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _AtkObject AtkObject;
typedef struct _AtkObjectClass AtkObjectClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Atk
{ class Object_Class; } // namespace Atk
#endif //DOXYGEN_SHOULD_SKIP_THIS

namespace Atk
{


/** @addtogroup atkmmEnums atkmm Enums and Flags */

/** 
 *
 * @ingroup atkmmEnums
 */
enum Role
{
  ROLE_INVALID,
  ROLE_ACCEL_LABEL,
  ROLE_ALERT,
  ROLE_ANIMATION,
  ROLE_ARROW,
  ROLE_CALENDAR,
  ROLE_CANVAS,
  ROLE_CHECK_BOX,
  ROLE_CHECK_MENU_ITEM,
  ROLE_COLOR_CHOOSER,
  ROLE_COLUMN_HEADER,
  ROLE_COMBO_BOX,
  ROLE_DATE_EDITOR,
  ROLE_DESKTOP_ICON,
  ROLE_DESKTOP_FRAME,
  ROLE_DIAL,
  ROLE_DIALOG,
  ROLE_DIRECTORY_PANE,
  ROLE_DRAWING_AREA,
  ROLE_FILE_CHOOSER,
  ROLE_FILLER,
  ROLE_FONT_CHOOSER,
  ROLE_FRAME,
  ROLE_GLASS_PANE,
  ROLE_HTML_CONTAINER,
  ROLE_ICON,
  ROLE_IMAGE,
  ROLE_INTERNAL_FRAME,
  ROLE_LABEL,
  ROLE_LAYERED_PANE,
  ROLE_LIST,
  ROLE_LIST_ITEM,
  ROLE_MENU,
  ROLE_MENU_BAR,
  ROLE_MENU_ITEM,
  ROLE_OPTION_PANE,
  ROLE_PAGE_TAB,
  ROLE_PAGE_TAB_LIST,
  ROLE_PANEL,
  ROLE_PASSWORD_TEXT,
  ROLE_POPUP_MENU,
  ROLE_PROGRESS_BAR,
  ROLE_PUSH_BUTTON,
  ROLE_RADIO_BUTTON,
  ROLE_RADIO_MENU_ITEM,
  ROLE_ROOT_PANE,
  ROLE_ROW_HEADER,
  ROLE_SCROLL_BAR,
  ROLE_SCROLL_PANE,
  ROLE_SEPARATOR,
  ROLE_SLIDER,
  ROLE_SPLIT_PANE,
  ROLE_SPIN_BUTTON,
  ROLE_STATUSBAR,
  ROLE_TABLE,
  ROLE_TABLE_CELL,
  ROLE_TABLE_COLUMN_HEADER,
  ROLE_TABLE_ROW_HEADER,
  ROLE_TEAR_OFF_MENU_ITEM,
  ROLE_TERMINAL,
  ROLE_TEXT,
  ROLE_TOGGLE_BUTTON,
  ROLE_TOOL_BAR,
  ROLE_TOOL_TIP,
  ROLE_TREE,
  ROLE_TREE_TABLE,
  ROLE_UNKNOWN,
  ROLE_VIEWPORT,
  ROLE_WINDOW,
  ROLE_HEADER,
  ROLE_FOOTER,
  ROLE_PARAGRAPH,
  ROLE_RULER,
  ROLE_APPLICATION,
  ROLE_AUTOCOMPLETE,
  ROLE_EDITBAR,
  ROLE_EMBEDDED,
  ROLE_ENTRY,
  ROLE_CHART,
  ROLE_CAPTION,
  ROLE_DOCUMENT_FRAME,
  ROLE_HEADING,
  ROLE_PAGE,
  ROLE_SECTION,
  ROLE_REDUNDANT_OBJECT,
  ROLE_FORM,
  ROLE_LINK,
  ROLE_INPUT_METHOD_WINDOW,
  ROLE_TABLE_ROW,
  ROLE_TREE_ITEM,
  ROLE_DOCUMENT_SPREADSHEET,
  ROLE_DOCUMENT_PRESENTATION,
  ROLE_DOCUMENT_TEXT,
  ROLE_DOCUMENT_WEB,
  ROLE_DOCUMENT_EMAIL,
  ROLE_COMMENT,
  ROLE_LIST_BOX,
  ROLE_GROUPING,
  ROLE_IMAGE_MAP,
  ROLE_NOTIFICATION,
  ROLE_INFO_BAR,
  ROLE_LEVEL_BAR,
  ROLE_TITLE_BAR,
  ROLE_BLOCK_QUOTE,
  ROLE_AUDIO,
  ROLE_VIDEO,
  ROLE_DEFINITION,
  ROLE_ARTICLE,
  ROLE_LANDMARK,
  ROLE_LOG,
  ROLE_MARQUEE,
  ROLE_MATH,
  ROLE_RATING,
  ROLE_TIMER,
  ROLE_DESCRIPTION_LIST,
  ROLE_DESCRIPTION_TERM,
  ROLE_DESCRIPTION_VALUE,
  ROLE_STATIC,
  ROLE_MATH_FRACTION,
  ROLE_MATH_ROOT,
  ROLE_SUBSCRIPT,
  ROLE_SUPERSCRIPT,
  ROLE_LAST_DEFINED
};

} // namespace Atk


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Atk::Role> : public Glib::Value_Enum<Atk::Role>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Atk
{


class RelationSet;
class Relation;
class StateSet;

typedef guint64 State;

/** The base object class for the Accessibility Toolkit API.
 * This class is the primary class for accessibility support via the Accessibility ToolKit (ATK). Objects which are
 * instances of Atk::Object (or instances of Atk::Object-derived types) are queried for properties which relate basic
 * (and generic) properties of a UI component such as name and description. Instances of Atk::Object may also be queried
 * as to whether they implement other ATK interfaces (e.g. Atk::Action, Atk::Component, etc.), as appropriate to the role
 * which a given UI component plays in a user interface.
 *
 * All UI components in an application which provide useful information or services to the user must provide corresponding
 * Atk::Object instances on request (in GTK+, for instance, usually on a call to Gtk::Widget::get_accessible()), either via
 * ATK support built into the toolkit for the widget class or ancestor class, or in the case of custom widgets, if the
 * inherited Atk::Object implementation is insufficient, via instances of a new Atk::Object subclass. 
 */

class Object : public Glib::Object
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef Object CppObjectType;
  typedef Object_Class CppClassType;
  typedef AtkObject BaseObjectType;
  typedef AtkObjectClass BaseClassType;

  // noncopyable
  Object(const Object&) = delete;
  Object& operator=(const Object&) = delete;

private:  friend class Object_Class;
  static CppClassType object_class_;

protected:
  explicit Object(const Glib::ConstructParams& construct_params);
  explicit Object(AtkObject* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:

  Object(Object&& src) noexcept;
  Object& operator=(Object&& src) noexcept;

  virtual ~Object() noexcept;

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type()      G_GNUC_CONST;

#ifndef DOXYGEN_SHOULD_SKIP_THIS


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  AtkObject*       gobj()       { return reinterpret_cast<AtkObject*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const AtkObject* gobj() const { return reinterpret_cast<AtkObject*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  AtkObject* gobj_copy();

private:

   // see wrap_new() implementation in object.ccg
  
public:

  
  /** Gets the accessible name of the accessible.
   * 
   * @return A character string representing the accessible name of the object.
   */
  Glib::ustring get_name() const;
  
  /** Gets the accessible description of the accessible.
   * 
   * @return A character string representing the accessible description
   * of the accessible.
   */
  Glib::ustring get_description() const;
  
  /** Gets the accessible parent of the accessible. By default this is
   * the one assigned with set_parent(), but it is assumed
   * that ATK implementors have ways to get the parent of the object
   * without the need of assigning it manually with
   * set_parent(), and will return it with this method.
   * 
   * If you are only interested on the parent assigned with
   * set_parent(), use peek_parent().
   * 
   * @return An Atk::Object representing the accessible
   * parent of the accessible.
   */
  Glib::RefPtr<Atk::Object> get_parent();
  
  /** Gets the number of accessible children of the accessible.
   * 
   * @return An integer representing the number of accessible children
   * of the accessible.
   */
  int get_n_accessible_children() const;
  
  /** Gets a reference to the specified accessible child of the object.
   * The accessible children are 0-based so the first accessible child is
   * at index 0, the second at index 1 and so on.
   * 
   * @param i A <tt>int</tt> representing the position of the child, starting from 0.
   * @return An Atk::Object representing the specified
   * accessible child of the accessible.
   */
  Glib::RefPtr<Atk::Object> get_accessible_child(int i);
  
  /** Gets the Atk::RelationSet associated with the object.
   * 
   * @return An Atk::RelationSet representing the relation set
   * of the object.
   */
  Glib::RefPtr<RelationSet> get_relation_set();
  
  /** Gets the role of the accessible.
   * 
   * @return An Atk::Role which is the role of the accessible.
   */
  Role get_role() const;
  
  /** Gets a reference to the state set of the accessible; the caller must
   * unreference it when it is no longer needed.
   * 
   * @return A reference to an Atk::StateSet which is the state
   * set of the accessible.
   */
  Glib::RefPtr<StateSet> get_state_set();
  
  /** Gets the 0-based index of this accessible in its parent; returns -1 if the
   * accessible does not have an accessible parent.
   * 
   * @return An integer which is the index of the accessible in its parent.
   */
  int get_index_in_parent();
  
  /** Sets the accessible name of the accessible. You can't set the name
   * to <tt>nullptr</tt>. This is reserved for the initial value. In this aspect
   * <tt>nullptr</tt> is similar to ATK_ROLE_UNKNOWN. If you want to set the name to
   * a empty value you can use "".
   * 
   * @param name A character string to be set as the accessible name.
   */
  void set_name(const Glib::ustring& name);
  
  /** Sets the accessible description of the accessible. You can't set
   * the description to <tt>nullptr</tt>. This is reserved for the initial value. In
   * this aspect <tt>nullptr</tt> is similar to ATK_ROLE_UNKNOWN. If you want to set
   * the name to a empty value you can use "".
   * 
   * @param description A character string to be set as the accessible description.
   */
  void set_description(const Glib::ustring& description);
  
  /** Sets the accessible parent of the accessible. @a parent can be <tt>nullptr</tt>.
   * 
   * @param parent An Atk::Object to be set as the accessible parent.
   */
  void set_parent(const Glib::RefPtr<Atk::Object>& parent);
  
  /** Sets the role of the accessible.
   * 
   * @param role An Atk::Role to be set as the role.
   */
  void set_role(Role role);
  //_WRAP_METHOD(guint connect_property_change_handler(AtkPropertyChangeHandler* handler), atk_object_connect_property_change_handler)
  //_WRAP_METHOD(void remove_property_change_handler(guint handler_id), atk_object_remove_property_change_handler)
  
  /** Emits a state-change signal for the specified state.
   * 
   * Note that as a general rule when the state of an existing object changes,
   * emitting a notification is expected.
   * 
   * @param state An Atk::State whose state is changed.
   * @param value A <tt>bool</tt> which indicates whether the state is being set on or off.
   */
  void notify_state_change(State state, bool value);

  
  /** Adds a relationship of the specified type with the specified target.
   * 
   * @param relationship The Atk::RelationType of the relation.
   * @param target The Atk::Object which is to be the target of the relation.
   * @return <tt>true</tt> if the relationship is added.
   */
  bool add_relationship(RelationType relationship, const Glib::RefPtr<Object>& target);
  
  /** Removes a relationship of the specified type with the specified target.
   * 
   * @param relationship The Atk::RelationType of the relation.
   * @param target The Atk::Object which is the target of the relation to be removed.
   * @return <tt>true</tt> if the relationship is removed.
   */
  bool remove_relationship(RelationType relationship, const Glib::RefPtr<Object>& target);

  
  /**
   * @par Slot Prototype:
   * <tt>void on_my_%children_changed(guint change_index, gpointer changed_child)</tt>
   *
   * The signal "children-changed" is emitted when a child is added or
   * removed form an object. It supports two details: "add" and
   * "remove"
   * 
   * @param change_index The index of the added or removed child. The value can be
   * -1. This is used if the value is not known by the implementor
   * when the child is added/removed or irrelevant.
   * @param changed_child A gpointer to the child AtkObject which was added or
   * removed. If the child was removed, it is possible that it is not
   * available for the implementor. In that case this pointer can be
   * <tt>nullptr</tt>.
   */

  Glib::SignalProxy2< void,guint,gpointer > signal_children_changed();

  
  /**
   * @par Slot Prototype:
   * <tt>void on_my_%focus_event(bool focus_in)</tt>
   *
   * The signal "focus-event" is emitted when an object gained or lost
   * focus.
   * 
   * Deprecated: 2.9.4: Use the Atk::Object::signal_state_change() signal instead.
   * 
   * @param focus_in A boolean value which indicates whether the object gained
   * or lost focus.
   */

  Glib::SignalProxy1< void,bool > signal_focus_event();

  
  /**
   * @par Slot Prototype:
   * <tt>void on_my_%property_change(AtkPropertyValues* values)</tt>
   *
   * The signal "property-change" is emitted when an object's property
   * value changes. @a values contains an Atk::PropertyValues with the name
   * and the new value of the property whose value has changed. Note
   * that, as with GObject notify, getting this signal does not
   * guarantee that the value of the property has actually changed; it
   * may also be emitted when the setter of the property is called to
   * reinstate the previous value.
   * 
   * Toolkit implementor note: ATK implementors should use
   * Glib::object_notify() to emit property-changed
   * notifications. Atk::Object::signal_property_changed() is needed by the
   * implementation of atk_add_global_event_listener() because GObject
   * notify doesn't support emission hooks.
   * 
   * @param values An Atk::PropertyValues containing the new value of the
   * property which changed.
   */

  Glib::SignalProxy1< void,AtkPropertyValues* > signal_property_change();


  /**
   * @par Slot Prototype:
   * <tt>void on_my_%state_change(const Glib::ustring& name, bool state_set)</tt>
   *
   * The "state-change" signal is emitted when an object's state
   * changes.  The detail value identifies the state type which has
   * changed.
   * 
   * @param name The name of the state which has changed.
   * @param state_set A boolean which indicates whether the state has been set or unset.
   */

  Glib::SignalProxy2< void,const Glib::ustring&,bool > signal_state_change();


  /**
   * @par Slot Prototype:
   * <tt>void on_my_%visible_data_changed()</tt>
   *
   * The "visible-data-changed" signal is emitted when the visual
   * appearance of the object changed.
   */

  Glib::SignalProxy0< void > signal_visible_data_changed();

  
  /**
   * @par Slot Prototype:
   * <tt>void on_my_%active_descendant_changed(void** child)</tt>
   *
   * The "active-descendant-changed" signal is emitted by an object
   * which has the state ATK_STATE_MANAGES_DESCENDANTS when the focus
   * object in the object changes. For instance, a table will emit the
   * signal when the cell in the table which has focus changes.
   * 
   * @param child The newly focused object.
   */

  Glib::SignalProxy1< void,void** > signal_active_descendant_changed();


  /** Object instance's name formatted for assistive technology access.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_accessible_name() ;

/** Object instance's name formatted for assistive technology access.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_accessible_name() const;

  /** Description of an object, formatted for assistive technology access.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_accessible_description() ;

/** Description of an object, formatted for assistive technology access.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_accessible_description() const;

  /** Parent of the current accessible as returned by atk_object_get_parent().
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Atk::Object> > property_accessible_parent() ;

/** Parent of the current accessible as returned by atk_object_get_parent().
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Atk::Object> > property_accessible_parent() const;

  /** Is used to notify that the value has changed.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< double > property_accessible_value() ;

/** Is used to notify that the value has changed.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< double > property_accessible_value() const;

  /** The accessible role of this object.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< int > property_accessible_role() ;

/** The accessible role of this object.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< int > property_accessible_role() const;

  /** The accessible layer of this object.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< int > property_accessible_component_layer() const;


  /** The accessible MDI value of this object.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< int > property_accessible_component_mdi_zorder() const;


  /** Is used to notify that the table caption has changed; this property should not be used. accessible-table-caption-object should be used instead.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_accessible_table_caption() ;

/** Is used to notify that the table caption has changed; this property should not be used. accessible-table-caption-object should be used instead.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_accessible_table_caption() const;

  /** Is used to notify that the table column description has changed.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_accessible_table_column_description() ;

/** Is used to notify that the table column description has changed.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_accessible_table_column_description() const;

  /** Is used to notify that the table column header has changed.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Atk::Object> > property_accessible_table_column_header() ;

/** Is used to notify that the table column header has changed.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Atk::Object> > property_accessible_table_column_header() const;

  /** Is used to notify that the table row description has changed.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_accessible_table_row_description() ;

/** Is used to notify that the table row description has changed.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_accessible_table_row_description() const;

  /** Is used to notify that the table row header has changed.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Atk::Object> > property_accessible_table_row_header() ;

/** Is used to notify that the table row header has changed.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Atk::Object> > property_accessible_table_row_header() const;

  /** Is used to notify that the table summary has changed.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Atk::Object> > property_accessible_table_summary() ;

/** Is used to notify that the table summary has changed.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Atk::Object> > property_accessible_table_summary() const;


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::
  /// This is a default handler for the signal signal_children_changed().
  virtual void on_children_changed(guint change_index, gpointer changed_child);
  /// This is a default handler for the signal signal_focus_event().
  virtual void on_focus_event(bool focus_in);
  /// This is a default handler for the signal signal_property_change().
  virtual void on_property_change(AtkPropertyValues* values);
  /// This is a default handler for the signal signal_state_change().
  virtual void on_state_change(const Glib::ustring& name, bool state_set);
  /// This is a default handler for the signal signal_visible_data_changed().
  virtual void on_visible_data_changed();
  /// This is a default handler for the signal signal_active_descendant_changed().
  virtual void on_active_descendant_changed(void** child);


};

} // namespace Atk


namespace Glib
{
  /** A Glib::wrap() method for this object.
   * 
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Atk::Object
   */
  Glib::RefPtr<Atk::Object> wrap(AtkObject* object, bool take_copy = false);
}


#endif /* _ATKMM_OBJECT_H */
