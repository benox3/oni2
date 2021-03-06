open Revery;
open Revery.UI;
open Oni_Core;
open Oni_Model;

let component = React.component("Menu");

let menuWidth = 400;
let menuRowHeight = 40;

let containerStyles = (theme: Theme.t) =>
  Style.[
    backgroundColor(theme.editorMenuBackground),
    color(theme.editorMenuForeground),
  ];

let menuItemStyle =
  Style.[
    fontSize(14),
    width(menuWidth - 50),
    cursor(Revery.MouseCursors.pointer),
  ];

let inputStyles = font =>
  Style.[
    border(~width=2, ~color=Color.rgba(0., 0., 0., 0.1)),
    backgroundColor(Color.rgba(0., 0., 0., 0.3)),
    width(menuWidth - 10),
    color(Colors.white),
    fontFamily(font),
  ];

let handleChange = str => GlobalContext.current().dispatch(MenuSearch(str));

let handleKeyDown = (event: NodeEvents.keyEventParams) =>
  switch (event) {
  | {key: Revery.Key.KEY_DOWN, _} =>
    GlobalContext.current().dispatch(MenuNextItem)
  | {key: Revery.Key.KEY_UP, _} =>
    GlobalContext.current().dispatch(MenuPreviousItem)
  | {key: Revery.Key.KEY_ESCAPE, _} =>
    GlobalContext.current().dispatch(SetInputControlMode(MenuFocus))
  | _ => ()
  };

let loseFocusOnClose = isOpen =>
  /**
   TODO: revery-ui/revery#412 if the menu is hidden abruptly the element is not automatically unfocused
   as revery is unaware the element is no longer in focus
 */
  (
    switch (Focus.focused, isOpen) {
    | ({contents: Some(_)}, false) => Focus.loseFocus()
    | (_, _) => ()
    }
  );

let onClick = () => {
  GlobalContext.current().dispatch(MenuSelect);
  GlobalContext.current().dispatch(SetInputControlMode(EditorTextFocus));
};

let onMouseOver = pos => GlobalContext.current().dispatch(MenuPosition(pos));

type fontT = Types.UiFont.t;

let getLabel = (command: Actions.menuCommand) => {
  switch (command.category) {
  | Some(v) => v ++ ": " ++ command.name
  | None => command.name
  };
};

let createElement =
    (
      ~children as _,
      ~font: fontT,
      ~menu: Menu.t,
      ~theme: Theme.t,
      ~configuration: Configuration.t,
      (),
    ) =>
  component(hooks => {
    let hooks =
      React.Hooks.effect(
        Always,
        () => {
          loseFocusOnClose(menu.isOpen);
          None;
        },
        hooks,
      );

    let commands = Job.getCompletedWork(menu.filterJob).uiFiltered;

    React.(
      hooks,
      menu.isOpen
        ? <AllowPointer>
            <OniBoxShadow configuration theme>
              <View style={containerStyles(theme)}>
                <View style=Style.[width(menuWidth), padding(5)]>
                  <OniInput
                    autofocus=true
                    placeholder="type here to search the menu"
                    cursorColor=Colors.white
                    style={inputStyles(font.fontFile)}
                    onChange=handleChange
                    onKeyDown=handleKeyDown
                  />
                </View>
                <View>
                  <FlatList
                    scrollY={float_of_int(menuRowHeight * menu.rowOffset)}
                    rowHeight=menuRowHeight
                    height={menuRowHeight * menu.maxRows}
                    width=menuWidth
                    count={Array.length(commands)}
                    render={index => {
                      let cmd = commands[index];
                      <MenuItem
                        onClick
                        theme
                        style=menuItemStyle
                        label={getLabel(cmd) ++ string_of_int(menu.rowOffset) ++ " selecteditem" ++ string_of_int(menu.selectedItem)}
                        icon={cmd.icon}
                        onMouseOver={_ => onMouseOver(index)}
                        selected={index == menu.selectedItem}
                      />;
                    }}
                  />
                </View>
              </View>
            </OniBoxShadow>
          </AllowPointer>
        : React.listToElement([]),
    );
  });
